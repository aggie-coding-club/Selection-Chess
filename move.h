#ifndef MOVE_H
#define MOVE_H

#include "constants.h"
#include "tokenizer.h"

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

        TileMove (Coords _selBottomLeft, Coords _selTopRight, SignedCoords _translation) : TileMove() {
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
        std::vector<Coords> m_deleteCoords;

        // Constructor for multiple coords
        TileDeletion (std::vector<Coords> _deleteCoords) : TileDeletion() {
            m_deleteCoords = _deleteCoords;
        }
        // Constructor for one coord
        TileDeletion (Coords _deleteCoord) : TileDeletion() {
            m_deleteCoords.push_back(_deleteCoord);
        }
        TileDeletion() {
            m_type = TILE_DELETION;
        }

        std::string algebraic();
};

std::unique_ptr<Move> readAlgebraic(std::string _algebra);

std::string coordsToAlgebraic(Coords _coords, Coords _offset=std::make_pair(0,0));
Coords algebraicToCoords(std::string _algebra, Coords _offset=std::make_pair(0,0));

std::string signedCoordsToAlgebraic(SignedCoords _coords);

class AlgebraicTokenizer : public AbstractTokenizer {
    public:
        AlgebraicTokenizer(std::string _string) : AbstractTokenizer(_string) {}
        // returns the next lexeme in the algebraic string. If we have reached the end of the string, return empty string.
        std::string next();
        // assuming the next sequence is a coords (e.g. ab15), return the coord
        Coords nextCoords();
        // assuming the next sequence is a signedcoords (e.g. +10-2), return the signed coord
        SignedCoords nextSignedCoords();
};


unsigned int lettersToInt(std::string _letters);
std::string intToLetters(unsigned int _int);

#endif