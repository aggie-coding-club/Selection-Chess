#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

#include "constants.h"

class Board {
    public:
        short m_movesSinceLastCapture; // 50 move rule
        bool m_turnWhite; // whose turn it is
        short m_material; // changed material score to just be material for both

        /** 
         * Creates a new board from SFEN.
         */
        // virtual Board(const std::string _sfen) = 0;

        /**
         * Boards are equal if all independent fields except moveHistory are equal. 
         * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
         * for most practical purposes. Really, this is just here to test if the hash function is working.
         */
        virtual bool operator==(const Board& _other) const = 0;

        /** 
         * Print the current tiles and pieces in a nice ASCII format.
         */
        virtual std::string getAsciiBoard(bool _showCoords = false, size_t _width = 3, size_t _height = 1, char _tileFillChar = '`') = 0;

        /**
         * Gets the size of the minimum rectangle needed to surround this board in its current configuration.
         */
        virtual std::pair<size_t, size_t> getDimensions() const = 0;

        /**
         * Gets the piece at the rank and file, zero indexed from current bounds.
         */
        virtual PieceEnum getPiece(size_t _r, size_t _f) const = 0;

        /**
         * Attempt to move selection of tiles (_selectMinR, _selectMinF)-(_selectMaxR, _selectMaxF) to the new coords (_goalMinR, _goalMinF).
         *                                        lower left corner           upper right corner                         lower left corner
         * Returns false if the move is illegal.
         */
        virtual bool moveSelection(size_t _selectMinR, size_t _selectMinF, size_t _selectMaxR, size_t _selectMaxF, size_t _goalMinR, size_t _goalMinF) = 0;

        /**
         * Attempt to move piece at (_startR, _startF) to the new coords (_goalR, _goalF).
         * Returns false if the move is illegal.
         */
        virtual bool movePiece(size_t _startR, size_t _startF, size_t _goalR, size_t _goalF) = 0;

        /**
         * Undoes the last move(s) made on this board.
         */
        virtual bool undo(size_t _numMoves=1) = 0;

        /**
         * Gets the hash of this configuration.
         */
        virtual uint64_t getHash() const = 0;

};

#endif