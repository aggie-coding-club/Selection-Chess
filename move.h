#ifndef MOVE_H
#define MOVE_H

#include "constants.h"

#include <sstream>
#include <vector>
#include <iostream>
#include <memory>

// Note: all of this object-orientatedness around the move class is for readibility.
// I bet you can pack this functionality into a single Move object and not have to use the heap,
// but any way I can think of to do that is not going to both clean looking and easily understandable.

// FIXME: Probably should use smart pointers for moves

typedef int MoveType;
enum : MoveType {INVALID_MOVE, PIECE_MOVE, TILE_MOVE, TILE_DELETION};

// Abstract class for moves
class Move {
    public:
        virtual std::string algebraic() {
            return "[INVALID MOVE]";
        };
        MoveType m_type = INVALID_MOVE;
};

// When a piece is moved. This is the normal type of move.
class PieceMove : public Move {
    public:
        Coords m_startPos;
        Coords m_endPos;
        PieceEnum m_capture;

        PieceMove (Coords _startPos, Coords _endPos) : PieceMove() { 
            m_startPos =_startPos;
            m_endPos =_endPos;
            m_capture =EMPTY;
        }
        PieceMove () {
            m_type = PIECE_MOVE;
        }

        std::string algebraic();
};

// When a rectangular selection of tiles is moved.
class TileMove : public Move {
    public:
        // Bottom left corner of our selection, used to tell where our selection box is starting or minimum values of coords
        Coords m_selFirst;
        // Top right corner of our selection, used to determine how big our selection box is or maximum values of coords
        Coords m_selSecond;

        // Note: the ability to delete entire sections can be coded into here
        // // If true, ignore m_destFirst and just delete tiles in selection.
        // bool m_isDeletion;

        // Bottom left corner of the destination for our selection, use to tell where to move the board to
        Coords m_destFirst;

        TileMove (Coords _selBottomLeft, Coords _selTopRight, Coords _destBottomLeft)
         : m_selFirst(_selBottomLeft), m_selSecond(_selTopRight), m_destFirst(_destBottomLeft) {
            TileMove();
        }
        TileMove() {
            m_type = TILE_MOVE;
        }

        std::string algebraic();
};

// For cherry-picking tiles to delete, e.g. 'Delete a2, b6, and e10'.
class TileDeletion : public Move {
    public:
        std::vector<Coords> m_deleteCoords;

        // Constructor for multiple coords
        TileDeletion (std::vector<Coords> _deleteCoords) : m_deleteCoords(_deleteCoords) {
            TileDeletion();
        }
        // Constructor for one coord
        TileDeletion (Coords _deleteCoord) {
            m_deleteCoords.push_back(_deleteCoord);
            TileDeletion();
        }
        TileDeletion() {
            m_type = TILE_DELETION;
        }

        std::string algebraic();
};

std::unique_ptr<Move> readAlgebraic(std::string _algebra);

std::string coordsToAlgebraic(Coords _coords, Coords _offset=std::make_pair(0,0));
Coords algebraicToCoords(std::string _algebra, Coords _offset=std::make_pair(0,0));

//TODO: move this class somewhere better, and make Tokenizer used in xboard commands a child of it
class AbstractTokenizer {
    protected:
        std::stringstream m_stream;
        std::string m_peeked;
        bool m_hasPeeked = false;
    public:
        AbstractTokenizer(std::string _string) : m_stream(_string) { };
        virtual std::string next() = 0;
        std::string peek();
};

class AlgebraicTokenizer : public AbstractTokenizer {
    public:
        AlgebraicTokenizer(std::string _string) : AbstractTokenizer(_string) {}
        std::string next();
};


unsigned int lettersToInt(std::string _letters);
std::string intToLetters(unsigned int _int);

#endif