#include <string>
#include <map>

#include "constants.h"
#include "board.h"

#include "utils.h"

DirectionEnum flipDirection(DirectionEnum _dir) {
    if (_dir & 1) { // if odd
        return _dir - 1;
    } else {
        return _dir + 1;
    }
}

char getCharFromPiece(PieceEnum _enumValue, char _empty, char _invalid) {
    if (_enumValue == EMPTY) {
        return _empty;
    }
    if (_enumValue == INVALID) {
        return _invalid;
    }
    return PIECE_LETTERS[_enumValue];
}

PieceEnum getPieceFromChar(char _char, char _empty) {
    if (_char == _empty) {
        return EMPTY;
    }
    std::size_t found = PIECE_LETTERS.find(_char);
    if (found == std::string::npos) {
        return INVALID;
    }
    return found;
}

// TODO: is there a better way to do this than just a big switch?
std::string getUnicodeCharFromEnum(PieceEnum _enumValue, std::string _empty) {
    // This could be put into constants.h?
    std::map<PieceEnum, std::string> unicodePieceChars = {
        {EMPTY, _empty},
        {W_PAWN, "♙"},
        {B_PAWN, "♟︎"},
        {W_ROOK, "♖"},
        {B_ROOK, "♜"},
        {W_KNIGHT, "♘"},
        {B_KNIGHT, "♞"},
        {W_BISHOP, "♗"},
        {B_BISHOP, "♝"},
        {W_QUEEN, "♕"},
        {B_QUEEN, "♛"},
        {W_KING, "♔"},
        {B_KING, "♚"},
        {INVALID, "?"} // not sure how this is intended to work
    };
    if (unicodePieceChars.count(_enumValue) > 0) {
        return unicodePieceChars[_enumValue];
    }
    return "?";
}

char getCharFromDirection(DirectionEnum _dir) {
    std::map<DirectionEnum, char> directionChars = {
        {LEFT, 'L'},
        {RIGHT, 'R'},
        {UP, 'U'},
        {DOWN, 'D'},
        {DOWN_LEFT, 'l'},
        {DOWN_RIGHT, 'r'},
        {UP_LEFT, '\\'},
        {UP_RIGHT, '/'}
    };
    if (directionChars.count(_dir) > 0) {
        return directionChars[_dir];
    }
    return '?';
}

// Throw away stream inputs. We use this for hiding debugging outputs in the 'dout' macro
// TODO: See if there is a better implementation to debugging prints, which still uses ostream syntax like cout.
class NullBuffer : public std::streambuf {
    public:
        int overflow(int c) { return c; }
};
NullBuffer null_buffer;
std::ostream g_nullout(&null_buffer);