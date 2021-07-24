#ifndef ARRAY_BOARD_H
#define ARRAY_BOARD_H

#include "constants.h"
#include "board.h"
#include "move.h"
#include "modular_int.hpp"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

extern unsigned int ABModulus;
typedef ModularInt<&ABModulus> ABModInt;
typedef std::pair<ABModInt, ABModInt> ABModCoords;

class ArrayBoard : public Board {
    public:
        PieceEnum* m_grid = nullptr;
        // max number of tiles the grid can contain, and also the length and height of the grid.
        size_t m_grid_size; 

        // minimum x and y of this board. Because of wrap-around, the literal min integer value is not 
        // guaranteed to be the furtherest "left" or "down".
        // Also note that these overshadow variables of Board, which are of type Coords. //FIXME: this is bad design and should be addressed
        ABModCoords m_minCoords;
        ABModCoords m_maxCoords;
        
        /** For looking up where pieces are at by their type and color. 
         * Organized in same order as PieceEnum: 
         *      since PieceEnum starts pieces on 1, the zero element of this is a placeholder.
        */
        std::vector<ABModCoords> m_pieceLocations[2 * NUM_PIECE_TYPES+1];

        explicit ArrayBoard(Ruleset& _ruleset) : Board(_ruleset) { };
        void init(const std::string _sfen);
        ArrayBoard(Ruleset& _ruleset, const std::string _sfen) : ArrayBoard(_ruleset) {
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
        bool updatePieceInPL(PieceEnum _piece, ABModCoords _oldLocation, ABModCoords _newLocation);

        /** 
         * Remove the piece at location for type piece.
         * Returns false if it does not find such a piece to remove.
         */
        bool removePieceFromPL(PieceEnum _piece, ABModCoords _location);
        /** 
         * Adds the piece at location for type piece.
         */
        void addPieceToPL(PieceEnum _piece, ABModCoords _location) {
            m_pieceLocations[_piece].push_back(_location);
        }

        PieceEnum getPiece(DModCoords _coords) const { // TODO: replace other calls with this one, or vice versa
            return m_grid[toIndex(toInternalCoords(dModCoordsToStandard(_coords)))];
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
                for (ABModCoords& t : m_pieceLocations[i]) {
                    result += PIECE_LETTERS[m_grid[toIndex(t)]];
                    result += " ";
                }
                result += "} ";
            }
            result += "]";
            return result;
        }

        std::vector<std::unique_ptr<Move>> getMoves(PieceColor _color);

        int staticEvaluation();

        // Just print the entire contents of the array as-is.
        // For debugging purposes only.
        std::string dumpAsciiArray() const;

        // Gets the index of m_grid corresponding to _coords. Takes internal Coords as input.
        size_t toIndex(ABModCoords _coords) const;

        // Convert external coords to internal, e.g. (0,0) will be converted to m_minCoords
        ABModCoords toInternalCoords(Coords _extern) const;
        // Convert internal coords to external, e.g. m_minCoords will be converted to (0,0)
        Coords toStandardCoords(ABModCoords _intern) const;

    protected: //TODO: sort some more stuff into protected?
        // check if adding a tile at _new ABModCoords will update m_minCoords or m_maxCoords
        void updateExtrema(const ABModCoords& _new);

        // Get the next coords at or after _start, sorted by row order. Assumes board is non-empty
        ABModCoords nextTileByRowOrder(const ABModCoords& _start, bool _reverse=false, bool _colReversed=false) const;
        // Get the next coords at or after _start, sorted by column order. Assumes board is non-empty
        ABModCoords nextTileByColOrder(const ABModCoords& _start, bool _reverse=false, bool _rowReversed=false) const;

        // // 'Less than' comparators for comparing ABModCoords.
        // bool compareFileInts(const ABModInt& _a, const ABModInt& _b) const {
        //     return _a.lessThan(_b, m_minCoords.first);
        // }
        // bool compareRankInts(const ABModInt& _a, const ABModInt& _b) const {
        //     return _a.lessThan(_b, m_minCoords.second);
        // }
        // Alternative version of above function
        inline bool compareFileCoords(const ABModCoords& _a, const ABModCoords& _b) const {
            return _a.first.lessThan(_b.first, m_minCoords.first);
        }
        inline bool compareRankCoords(const ABModCoords& _a, const ABModCoords& _b) const {
            return _a.second.lessThan(_b.second, m_minCoords.second);
        }

        // Copy the tiles from bottom left _bl to top right _tr into a standardarray. If _cut=true, remove the original copied section.
        StandardArray getSelection(const ABModCoords& _bl, const ABModCoords& _tr, bool _cut=false);

        // Paste the tiles from _sa into our grid, matching bottom left _bl to bottom left of _sa.
        // WARNING: does not update m_minCoords, m_maxCoords, pieceList, or any other member besides m_grid!
        void paste(const StandardArray& _sa, const ABModCoords& _bl);

        // TODO: Do these MO functions generalize well enough to be moved into board.h?

        // Checks if the move could have been generated from current MoveOption
        bool moveIsFromMO(std::shared_ptr<Move> _move, const MoveOption& _mo);
        bool moveIsFromMO(std::shared_ptr<Move> _move, const LeapMoveOption& _mo);
        bool moveIsFromMO(std::shared_ptr<Move> _move, const SlideMoveOption& _mo);
        // Returns list of moves that the piece at _pieceCoords can make using this MoveOption on the current _board.
        std::vector<std::unique_ptr<Move>> getMovesFromMO(ABModCoords& _pieceCoords, const MoveOption& _mo);
        std::vector<std::unique_ptr<Move>> getMovesFromMO(ABModCoords& _pieceCoords, const LeapMoveOption& _mo);
        std::vector<std::unique_ptr<Move>> getMovesFromMO(ABModCoords& _pieceCoords, const SlideMoveOption& _mo);


};

inline ABModCoords& operator+=(ABModCoords& _mc1, const SignedCoords& _diff) {
    _mc1.first += _diff.first;
    _mc1.second += _diff.second;
    return _mc1;
}
inline ABModCoords& operator-=(ABModCoords& _mc1, const SignedCoords& _diff) {
    _mc1.first -= _diff.first;
    _mc1.second -= _diff.second;
    return _mc1;
}
inline ABModCoords operator+(ABModCoords _mc1, const SignedCoords& _diff) {
    return _mc1 += _diff;
}
inline ABModCoords operator+(const SignedCoords& _diff, ABModCoords _mc1) {
    return _mc1 += _diff;
}
inline ABModCoords operator-(ABModCoords _mc1, const SignedCoords& _diff) {
    return _mc1 -= _diff;
}

#endif