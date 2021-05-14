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
         * Update the position oldLocation to be newLocation for type piece.
         * Give (only) one of the locations as (-1,-1) for adding or removing a piece.
         * Returns false if it does not find such a piece to update.
         */
        bool updatePieceInPL(PieceEnum _piece, ModCoords _oldLocation, ModCoords _newLocation);

        PieceEnum getPiece(size_t _f, size_t _r) const {
            return m_grid[_f + _r * m_grid_size];
        };

        bool apply(std::shared_ptr<Move> _move) {
            //TODO: implement
            return false;
        };

        bool undo(std::shared_ptr<Move> _move) {
            //TODO: implement
            return false;
        };

        Coords getDimensions() const {
            // Just taking the m_value here is OK because this distance is not affected by modulus, so it is guaranteed result to be a normal positive number.
            return std::make_pair((m_maxCoords.first - m_minCoords.first + 1).m_value, (m_maxCoords.second - m_minCoords.second + 1).m_value);
        };

        std::vector<std::unique_ptr<Move>> getMoves(PieceColor _color) {
            //TODO: implement
            std::vector<std::unique_ptr<Move>> a;
            return a;
        };

        int staticEvaluation() {
            return 0;
        }

        // Just print the entire contents of the array as-is.
        // For debugging purposes only.
        std::string dumpAsciiArray();

        // Gets the index of m_grid corresponding to _coords. 
        // If _useInternal=true, it will get (0,0) as 0th index.
        // If _useInternal=false, it will get m_minCoords as 0th index.
        size_t toIndex(ModCoords _coords, bool _useInternal);
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

#endif