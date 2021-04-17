#ifndef ARRAY_BOARD_H
#define ARRAY_BOARD_H

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

#include "constants.h"
#include "board.h"

class ArrayBoard : public Board {
    public:
        /* ------- independent fields, provide necessary information about board ------- */
        PieceEnum* m_grid;
        size_t m_grid_size; // max number of tiles the grid can contain, and also the length and height of the grid.

        // minimum x and y of this board. Because of wrap-around, the literal min integer value is not 
        // guaranteed to be the furtherest "left" or "down".
        coords m_minCoords;
        coords m_maxCoords;

        short m_movesSinceLastCapture; // 50 move rule
        bool m_turnWhite; // whose turn it is
        // std::stack<Move> moveHistory; // list of moves applied to starting FEN.

        /* ------- dependent fields, store information about board that is derived from independent fields -------- */
        short m_material; // changed material score to just be material for both
        uint64_t m_hashCode;
        
        /** For looking up where pieces are at by their type and color. */
        std::vector<coords> pieceLocations[2 * NUM_PIECE_TYPES];

        /** 
         * Creates a new board from SFEN.
         */
        ArrayBoard(const std::string _sfen);

        /**
         * Boards are equal if all independent fields except moveHistory are equal. 
         * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
         * for most practical purposes. Really, this is just here to test if the hash function is working.
         */
        bool operator==(const Board& _other) const;

        /** 
         * Update the position oldLocation to be newLocation for type piece.
         * Give (only) one of the locations as (-1,-1) for adding or removing a piece.
         * Returns false if it does not find such a piece to update.
         */
        bool updatePieceInPL(PieceEnum _piece, coords _oldLocation, coords _newLocation);

        /** 
         * Print the current tiles and pieces in a nice ASCII format.
         * @warning: If the board's tiles have errors in their adjacents arrays, this method may have unexpected behavior
         */
        std::string getAsciiBoard();

        coords getDimensions() const {
            //TODO: implement
            return std::make_pair(0u,0u);
        };

        PieceEnum getPiece(size_t _f, size_t _r) const {
            return m_grid[_f + _r * m_grid_size];
        };

        bool moveSelection(coords _select1, coords _select2, coords _goal1) {
            //TODO: implement
            return false;
        };

        bool movePiece(coords _start, coords _goal) {
            //TODO: implement
            return false;
        };

        bool undo(size_t _numMoves=1) {
            //TODO: implement
            return false;
        };

        uint64_t getHash() const {
            //TODO: implement
            return 0u;
        };
};

#endif