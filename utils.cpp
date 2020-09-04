#include <string>

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
    switch (_enumValue) {
    case EMPTY:
        return _empty;
        break;
    case W_PAWN:
        return "♙";
        break;
    case B_PAWN:
        return "♟︎";
        break;
    case W_ROOK:
        return "♖";
        break;
    case B_ROOK:
        return "♜";
        break;
    case W_KNIGHT:
        return "♘";
        break;
    case B_KNIGHT:
        return "♞";
        break;
    case W_BISHOP:
        return "♗";
        break;
    case B_BISHOP:
        return "♝";
        break;
    case W_QUEEN:
        return "♕";
        break;
    case B_QUEEN:
        return "♛";
        break;
    case W_KING:
        return "♔";
        break;
    case B_KING:
        return "♚";
        break;
    default:
        return "?";
        break;
    }
}

char getCharFromDirection(DirectionEnum _dir) {
    switch (_dir) {
    case LEFT:
        return 'L';
        break;
    case RIGHT:
        return 'R';
        break;
    case UP:
        return 'U';
        break;
    case DOWN:
        return 'D';
        break;
    case DOWN_LEFT:
        return 'l';
        break;
    case DOWN_RIGHT:
        return 'r';
        break;
    case UP_LEFT:
        return '\\';
        break;
    case UP_RIGHT:
        return '/';
        break;
    default:
        return '?';
        break;
    }
}

// Throw away stream inputs. We use this for hiding debugging outputs in the 'dout' macro
// TODO: See if there is a better implementation to debugging prints, which still uses ostream syntax like cout.
class NullBuffer : public std::streambuf {
    public:
        int overflow(int c) { return c; }
};
NullBuffer null_buffer;
std::ostream g_nullout(&null_buffer);

