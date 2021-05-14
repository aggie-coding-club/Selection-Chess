#ifndef ARRAY_BOARD_H
#define ARRAY_BOARD_H

#include "constants.h"
#include "board.h"
#include "move.h"
#include "modular_int.h"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

typedef std::pair<ModularInt, ModularInt> ModCoords;

class ArrayBoard : public Board {
    public:
        PieceEnum* m_grid = nullptr;
        // max number of tiles the grid can contain, and also the length and height of the grid.
        size_t m_grid_size; 

        // minimum x and y of this board. Because of wrap-around, the literal min integer value is not 
        // guaranteed to be the furtherest "left" or "down".
        // Also note that these overshadow variables of Board, which are of type Coords. //FIXME: this is bad design and should be addressed
        ModCoords m_minCoords;
        ModCoords m_maxCoords;
        
        /** For looking up where pieces are at by their type and color. 
         * Organized in same order as PieceEnum: 
         *      since PieceEnum starts pieces on 1, the zero element of this is a placeholder.
        */
        std::vector<ModCoords> m_pieceLocations[2 * NUM_PIECE_TYPES+1];

        ArrayBoard() { };
        void init(const std::string _sfen);
        ArrayBoard(const std::string _sfen) : ArrayBoard() {
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
        bool updatePieceInPL(PieceEnum _piece, ModCoords _oldLocation, ModCoords _newLocation);

        /** 
         * Remove the piece at location for type piece.
         * Returns false if it does not find such a piece to remove.
         */
        bool removePieceFromPL(PieceEnum _piece, ModCoords _location);
        /** 
         * Adds the piece at location for type piece.
         */
        void addPieceToPL(PieceEnum _piece, ModCoords _location) {
            m_pieceLocations[_piece].push_back(_location);
        }


        PieceEnum getPiece(size_t _f, size_t _r) const {
            return m_grid[_f + _r * m_grid_size];
        };

        bool apply(std::shared_ptr<Move> _move);
        bool apply(std::shared_ptr<PieceMove> _move);
        bool apply(std::shared_ptr<TileMove> _move);
        bool apply(std::shared_ptr<TileDeletion> _move);

        bool undo(std::shared_ptr<Move> _move);
        bool undo(std::shared_ptr<PieceMove> _move);
        bool undo(std::shared_ptr<TileMove> _move);
        bool undo(std::shared_ptr<TileDeletion> _move);

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
                for (ModCoords& t : m_pieceLocations[i]) {
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
        std::string dumpAsciiArray();

        // Gets the index of m_grid corresponding to _coords. Takes internal Coords as input.
        size_t toIndex(ModCoords _coords);

        // Convert external coords to internal, e.g. (0,0) will be converted to m_minCoords
        ModCoords toInternalCoords(Coords _extern);
        // Convert internal coords to external, e.g. m_minCoords will be converted to (0,0)
        Coords toExternalCoords(ModCoords _intern);

    protected: //TODO: sort some more stuff into protected?
        // check if adding a tile at _new ModCoords will update m_minCoords or m_maxCoords
        void ArrayBoard::updateExtrema(const ModCoords& _new);

        // // 'Less than' comparators for comparing ModCoords.
        // bool compareFileInts(const ModularInt& _a, const ModularInt& _b) const {
        //     return _a.lessThan(_b, m_minCoords.first);
        // }
        // bool compareRankInts(const ModularInt& _a, const ModularInt& _b) const {
        //     return _a.lessThan(_b, m_minCoords.second);
        // }
        // Alternative version of above function
        inline bool compareFileCoords(const ModCoords& _a, const ModCoords& _b) const {
            return _a.first.lessThan(_b.first, m_minCoords.first);
        }
        inline bool compareRankCoords(const ModCoords& _a, const ModCoords& _b) const {
            return _a.second.lessThan(_b.second, m_minCoords.second);
        }

};

inline bool operator==(const ModCoords& _mc1, const ModCoords& _mc2) {
    return (_mc1.first == _mc2.first) && (_mc1.second == _mc2.second);
}
inline ModCoords& operator+=(ModCoords& _mc1, const SignedCoords& _diff) {
    _mc1.first += _diff.first;
    _mc1.second += _diff.second;
    return _mc1;
}
inline ModCoords& operator-=(ModCoords& _mc1, const SignedCoords& _diff) {
    _mc1.first -= _diff.first;
    _mc1.second -= _diff.second;
    return _mc1;
}
inline ModCoords operator+(ModCoords _mc1, const SignedCoords& _diff) {
    return _mc1 += _diff;
}
inline ModCoords operator+(const SignedCoords& _diff, ModCoords _mc1) {
    return _mc1 += _diff;
}
inline ModCoords operator-(ModCoords _mc1, const SignedCoords& _diff) {
    return _mc1 -= _diff;
}

#endif