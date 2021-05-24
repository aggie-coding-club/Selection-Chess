#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "utils.h"
#include "move.h"
#include "ruleset.h"

#include <vector>

class BoardPrintSettings {
    public:
    bool m_showCoords = false;
    size_t m_width = 3;
    size_t m_height = 1;
    char m_tileFillChar = '`';

    BoardPrintSettings(bool _showCoords = true, size_t _width = 3, size_t _height = 1, char _tileFillChar = '`') :  
    m_showCoords(_showCoords), m_width(_width), m_height(_height), m_tileFillChar(_tileFillChar) {}
};

// TODO: I feel like this object can be designed better...
// TODO: implement functionality around this object
class StandardArray {
    public:
    Coords m_dimensions;
    std::vector<PieceEnum> m_array;

    StandardArray();
    // Creates a new empty array of size
    StandardArray(Coords _size);
    // Creates new array from sfen
    StandardArray(std::string _sfen);
    ~StandardArray();

    // Just print the entire contents of the array as-is.
    // For debugging purposes only.
    std::string dumpAsciiArray();
};

class Board {
    public:
        BoardPrintSettings m_printSettings;

        Board(Ruleset& _ruleset) : m_displayCoordsZero(std::make_pair(0,0)), m_rules(_ruleset) { }

        // Resets board from SFEN.
        virtual void init(const std::string _sfen) = 0;
        // virtual Board(const std::string _sfen) = 0;
        // minimum x and y of this board. Because of wrap-around, the literal min integer value is not 
        // guaranteed to be the furtherest "left" or "down".
        Coords m_minCoords; // TODO: consider removing this? This seems too implementation specific now...
        Coords m_maxCoords;

        // Which displayCoords map to standardArray's (0, 0)
        // Usually, should update whenever minCoords update.
        DModCoords m_displayCoordsZero;

        int m_material; // changed material score to just be material for both
        uint64_t m_hashCode;

        const Ruleset& m_rules;

        virtual std::string toSfen();

        /**
         * Boards are equal if all independent fields except moveHistory are equal. 
         * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
         * for most practical purposes. Really, this is just here to test if the hash function is working.
         */
        virtual bool operator==(const Board& _other) const;

        DModCoords standardToDModCoords(Coords _standard);
        Coords dModCoordsToStandard(DModCoords _dMod);

        /** 
         * Print the current tiles and pieces in a nice ASCII format.
         */
        virtual std::string getAsciiBoard();

        /**
         * Gets the size of the minimum rectangle needed to surround this board in its current configuration.
         */
        virtual Coords getDimensions() const {
            return std::make_pair(m_maxCoords.first - m_minCoords.first + 1, m_maxCoords.second - m_minCoords.second + 1);
        };

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

        virtual bool apply(std::shared_ptr<Move> _move) = 0;

        /**
         * Undoes the last move(s) made on this board.
         */
        virtual bool undo(std::shared_ptr<Move> _move) = 0;

        /**
         * Gets the hash of this configuration.
         */
        virtual uint64_t getHash() const;

        virtual int staticEvaluation() = 0;

        // For debugging purposes.
        virtual std::string printPieces() {
            return "[Lol I'm just the parent class, I can't do this]";
        }

        virtual std::vector<std::unique_ptr<Move>> getMoves(PieceColor _color) = 0;

        // Gets the standard array of this board. This is the smallest sized array (namely, size getDimensions())
        // that contains all pieces. Indexed by (file + rank*getDimensions().first). Used for printing, hashing, etc.
        // TODO: once the standardArrays are better working, then require this as a function
        virtual StandardArray standardArray() = 0;

        //TODO: unvirtual getAscii and toSfen using new standardArray

        // TODO: add some functions for moving tiles
};

#endif