#ifndef SPARSE_BOARD_H
#define SPARSE_BOARD_H

#include "constants.h"
#include "board.h"
#include "move.h"
#include "modular_int.hpp"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>
#include <map>

class SparseBoard : public Board {
    public:
        std::map<DModCoords, PieceEnum> m_grid;

        // TODO: consider using redunant arrays, one row major and one column major, so that we can iterate down rows/columns easily.
        // std::map<DModCoords, PieceEnum> m_rowMajArray;
        // std::map<DModCoords, PieceEnum> m_colMajArray;
        
        /** For looking up where pieces are at by their type and color. 
         * Organized in same order as PieceEnum: 
         *      since PieceEnum starts pieces on 1, the zero element of this is a placeholder.
        */
        std::vector<DModCoords> m_pieceLocations[2 * NUM_PIECE_TYPES+1];

        explicit SparseBoard(Ruleset& _ruleset) : Board(_ruleset) { };
        void init(const std::string _sfen);
        SparseBoard(Ruleset& _ruleset, const std::string _sfen) : SparseBoard(_ruleset) {
            init(_sfen);
        }

        StandardArray standardArray();

        /** 
         * Clears entire piece list
         */
        void resetPL() {
            for (PieceEnum _piece = 0; _piece < 2 * NUM_PIECE_TYPES + 1; _piece++) { // loop for all lists
                m_pieceLocations[_piece].clear();
            }
        }

        /** 
         * Update the position oldLocation to be newLocation for type piece.
         * Returns false if it does not find such a piece to update.
         */
        bool updatePieceInPL(PieceEnum _piece, DModCoords _oldLocation, DModCoords _newLocation);

        /** 
         * Remove the piece at location for type piece.
         * Returns false if it does not find such a piece to remove.
         */
        bool removePieceFromPL(PieceEnum _piece, DModCoords _location);
        /** 
         * Adds the piece at location for type piece.
         */
        void addPieceToPL(PieceEnum _piece, DModCoords _location) {
            m_pieceLocations[_piece].push_back(_location);
        }

        // Safer than just accessing the array, which throws exceptions on nontiles.
        PieceEnum getPiece(DModCoords _coords) const {
            auto it = m_grid.find(_coords);
            if (it != m_grid.end()) {
                return it->second;
            } else {
                return INVALID;
            }
        };

        bool apply(std::shared_ptr<Move> _move);
        bool apply(std::shared_ptr<PieceMove> _move);
        bool apply(std::shared_ptr<TileMove> _move);
        bool apply(std::shared_ptr<TileDeletion> _move);

        bool undo(std::shared_ptr<Move> _move);
        bool undo(std::shared_ptr<PieceMove> _move);
        bool undo(std::shared_ptr<TileMove> _move);
        bool undo(std::shared_ptr<TileDeletion> _move);

        bool isLegal(std::shared_ptr<Move> _move);
        bool isLegal(std::shared_ptr<PieceMove> _move);
        bool isLegal(std::shared_ptr<TileMove> _move);
        bool isLegal(std::shared_ptr<TileDeletion> _move);

        Coords getDimensions() const {
            // Just taking the m_value here is OK because this distance is not affected by modulus, so it is guaranteed result to be a normal positive number.
            return std::make_pair((m_maxCoords.first - m_minCoords.first + 1).m_value, (m_maxCoords.second - m_minCoords.second + 1).m_value);
        };

        // For debugging purposes. Prints all pieces in the m_pieceLocations list
        std::string printPieces() {
            std::string result = "[";
            for (int i = 1; i < NUM_PIECE_TYPES*2+1; i++) {
                result += PIECE_LETTERS[i];
                result += "=" + std::to_string(m_pieceLocations[i].size()) + "{";
                for (DModCoords& t : m_pieceLocations[i]) {
                    result += PIECE_LETTERS[getPiece(t)];
                    result += " ";
                }
                result += "} ";
            }
            result += "]";
            return result;
        }

        std::vector<std::unique_ptr<Move>> getMoves(PieceColor _color);

        int staticEvaluation();

    protected: //TODO: sort some more stuff into protected?
        // check if adding a tile at _new DModCoords will update m_minCoords or m_maxCoords
        void updateExtrema(const DModCoords& _new);

        // Get the next coords at or after _start, sorted by row order. Assumes board is non-empty
        DModCoords nextTileByRowOrder(const DModCoords& _start, bool _reverse=false, bool _colReversed=false) const;
        // Get the next coords at or after _start, sorted by column order. Assumes board is non-empty
        DModCoords nextTileByColOrder(const DModCoords& _start, bool _reverse=false, bool _rowReversed=false) const;

        // // 'Less than' comparators for comparing DModCoords.
        // bool compareFileInts(const ABModInt& _a, const ABModInt& _b) const {
        //     return _a.lessThan(_b, m_minCoords.first);
        // }
        // bool compareRankInts(const ABModInt& _a, const ABModInt& _b) const {
        //     return _a.lessThan(_b, m_minCoords.second);
        // }
        // Alternative version of above function
        inline bool compareFileCoords(const DModCoords& _a, const DModCoords& _b) const {
            return _a.first.lessThan(_b.first, m_minCoords.first);
        }
        inline bool compareRankCoords(const DModCoords& _a, const DModCoords& _b) const {
            return _a.second.lessThan(_b.second, m_minCoords.second);
        }

        // Copy the tiles from bottom left _bl to top right _tr into a standardarray. If _cut=true, remove the original copied section.
        StandardArray getSelection(const DModCoords& _bl, const DModCoords& _tr, bool _cut=false);

        // Paste the tiles from _sa into our grid, matching bottom left _bl to bottom left of _sa.
        // WARNING: does not update m_minCoords, m_maxCoords, pieceList, or any other member besides m_grid!
        void paste(const StandardArray& _sa, const DModCoords& _bl);

        // TODO: Do these MO functions generalize well enough to be moved into board.h?

        // Checks if the move could have been generated from current MoveOption
        bool moveIsFromMO(std::shared_ptr<Move> _move, const MoveOption& _mo);
        bool moveIsFromMO(std::shared_ptr<Move> _move, const LeapMoveOption& _mo);
        bool moveIsFromMO(std::shared_ptr<Move> _move, const SlideMoveOption& _mo);
        // Returns list of moves that the piece at _pieceCoords can make using this MoveOption on the current _board.
        std::vector<std::unique_ptr<Move>> getMovesFromMO(DModCoords& _pieceCoords, const MoveOption& _mo);
        std::vector<std::unique_ptr<Move>> getMovesFromMO(DModCoords& _pieceCoords, const LeapMoveOption& _mo);
        std::vector<std::unique_ptr<Move>> getMovesFromMO(DModCoords& _pieceCoords, const SlideMoveOption& _mo);


};
#endif