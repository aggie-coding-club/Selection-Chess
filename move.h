#ifndef MOVE_H
#define MOVE_H

#include "constants.h"
#include "tokenizer.h"
#include "modular_int.hpp"

#include <sstream>
#include <vector>
#include <iostream>
#include <memory>

// Note: all of this object-orientatedness around the move class is for readibility.
// I bet you can pack this functionality into a single Move object and not have to use the heap,
// but any way I can think of to do that is not going to both clean looking and easily understandable.

//TODO: surely there is a better place to put this
// Modulus used by Displayed alphabet coord.
extern unsigned int DAModulus;
// Modular Int used by displayed alphabet coord, i.e. a,...,z,aa,ab,...,zz
typedef ModularInt<&DAModulus> DAModInt;
// Modulus used by Displayed numeric coord.
extern unsigned int DDModulus;
// Modular Int used by displayed numberic coord, i.e. 0,...,999
typedef ModularInt<&DDModulus> DDModInt;
typedef std::pair<DAModInt, DDModInt> DModCoords;

// TODO: convert moves to use DModCoords.

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
        DModCoords m_startPos;
        DModCoords m_endPos;
        PieceEnum m_capture;

        PieceMove (DModCoords _startPos, DModCoords _endPos) : PieceMove() { 
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
        DModCoords m_selFirst;
        // Top right corner of our selection, used to determine how big our selection box is or maximum values of coords
        DModCoords m_selSecond;

        // Note: the ability to delete entire sections can be coded into here
        // // If true, ignore m_translation and just delete tiles in selection.
        // bool m_isDeletion;

        // Translation of selection
        SignedCoords m_translation;

        // The symmetry operators applied to this move.
        // The absolute value of this specifies how many rotations to apply,
        // and the sign specifies whether to mirror about the vertical after the rotations.
        // Valid values range from -4 to 4, excluding 0. (Note we don't use 0 because -0=0)
        // TODO: use this member
        int m_symmetry = 4;

        TileMove (DModCoords _selBottomLeft, DModCoords _selTopRight, SignedCoords _translation) : TileMove() {
            m_selFirst = _selBottomLeft;
            m_selSecond = _selTopRight;
            m_translation = _translation;
        }
        TileMove() {
            m_type = TILE_MOVE;
        }

        std::string algebraic();
};

// For cherry-picking tiles to delete, e.g. 'Delete a2, b6, and e10'.
class TileDeletion : public Move {
    public:
        std::vector<DModCoords> m_deleteCoords;

        // Constructor for multiple coords
        TileDeletion (std::vector<DModCoords> _deleteCoords) : TileDeletion() {
            m_deleteCoords = _deleteCoords;
        }
        // Constructor for one coord
        TileDeletion (DModCoords _deleteCoord) : TileDeletion() {
            m_deleteCoords.push_back(_deleteCoord);
        }
        TileDeletion() {
            m_type = TILE_DELETION;
        }

        std::string algebraic();
};

std::unique_ptr<Move> readAlgebraic(std::string _algebra);

std::string coordsToAlgebraic(DModCoords _coords, DModCoords _offset=std::make_pair(0,0));
DModCoords algebraicToCoords(std::string _algebra, DModCoords _offset=std::make_pair(0,0));

std::string signedCoordsToAlgebraic(SignedCoords _coords);

class AlgebraicTokenizer : public AbstractTokenizer {
    public:
        AlgebraicTokenizer(std::string _string) : AbstractTokenizer(_string) {}
        // returns the next lexeme in the algebraic string. If we have reached the end of the string, return empty string.
        std::string next();
        // assuming the next sequence is a coords (e.g. ab15), return the coord
        DModCoords nextCoords();
        // assuming the next sequence is a signedcoords (e.g. +10-2), return the signed coord
        SignedCoords nextSignedCoords();
};


DAModInt lettersToInt(std::string _letters);
std::string intToLetters(DAModInt _int);

// TODO: is there a more readable/concise way of doing this?
inline DModCoords& operator+=(DModCoords& _mc1, const SignedCoords& _diff) {
    _mc1.first += _diff.first;
    _mc1.second += _diff.second;
    return _mc1;
}
inline DModCoords& operator-=(DModCoords& _mc1, const SignedCoords& _diff) {
    _mc1.first -= _diff.first;
    _mc1.second -= _diff.second;
    return _mc1;
}
inline DModCoords operator+(DModCoords _mc1, const SignedCoords& _diff) {
    return _mc1 += _diff;
}
inline DModCoords operator+(const SignedCoords& _diff, DModCoords _mc1) {
    return _mc1 += _diff;
}
inline DModCoords operator-(DModCoords _mc1, const SignedCoords& _diff) {
    return _mc1 -= _diff;
}

#endif