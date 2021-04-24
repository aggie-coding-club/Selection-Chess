#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "utils.h"
#include "move.h"

class BoardPrintSettings {
    public:
    bool m_showCoords = false;
    size_t m_width = 3;
    size_t m_height = 1;
    char m_tileFillChar = '`';

    BoardPrintSettings(bool _showCoords = true, size_t _width = 3, size_t _height = 1, char _tileFillChar = '`') :  
    m_showCoords(_showCoords), m_width(_width), m_height(_height), m_tileFillChar(_tileFillChar) {}
};

class Board {
    public:
        BoardPrintSettings m_printSettings;
        /** 
         * Creates a new board from SFEN.
         */
        // virtual Board() = 0;
        virtual void init(const std::string _sfen) = 0;
        // virtual Board(const std::string _sfen) = 0;
        // minimum x and y of this board. Because of wrap-around, the literal min integer value is not 
        // guaranteed to be the furtherest "left" or "down".
        Coords m_minCoords;
        Coords m_maxCoords;

        int m_material; // changed material score to just be material for both
        uint64_t m_hashCode;

        virtual std::string toSfen() = 0;

        /**
         * Boards are equal if all independent fields except moveHistory are equal. 
         * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
         * for most practical purposes. Really, this is just here to test if the hash function is working.
         */
        virtual bool operator==(const Board& _other) const = 0;

        /** 
         * Print the current tiles and pieces in a nice ASCII format.
         */
        virtual std::string getAsciiBoard() = 0;

        /**
         * Gets the size of the minimum rectangle needed to surround this board in its current configuration.
         */
        virtual Coords getDimensions() const = 0;

        /**
         * Gets the piece at the rank and file, zero indexed from current bounds.
         */
        virtual PieceEnum getPiece(size_t _f, size_t _r) const = 0;

        // /**
        //  * Attempt to move selection of tiles (_selectMinR, _selectMinF)-(_selectMaxR, _selectMaxF) to the new coords (_goalMinR, _goalMinF).
        //  *                                        lower left corner           upper right corner                         lower left corner
        //  * Returns false if the move is illegal.
        //  */
        // virtual bool moveSelection(Coords _select1, Coords _select2, Coords _goal1) = 0;

        // /**
        //  * Attempt to move piece at (_startR, _startF) to the new Coords (_goalR, _goalF).
        //  * Returns false if the move is illegal.
        //  */
        // virtual bool movePiece(Coords _start, Coords _goal) = 0;

        virtual bool apply(Move _move) = 0;

        /**
         * Undoes the last move(s) made on this board.
         */
        virtual bool undo(Move _move) = 0;

        /**
         * Gets the hash of this configuration.
         */
        virtual uint64_t getHash() const = 0;

        // TODO: add some functions for moving tiles
};

#endif