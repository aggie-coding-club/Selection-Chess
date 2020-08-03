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

// TODO: is there a better way to do this than just a big switch?
char getCharFromEnum(PieceEnum _enumValue, char _empty) {
    switch (_enumValue) {
    case EMPTY:
        return _empty;
        break;
    case W_PAWN:
        return 'P';
        break;
    case B_PAWN:
        return 'p';
        break;
    case W_ROOK:
        return 'R';
        break;
    case B_ROOK:
        return 'r';
        break;
    case W_KNIGHT:
        return 'N';
        break;
    case B_KNIGHT:
        return 'n';
        break;
    case W_BISHOP:
        return 'B';
        break;
    case B_BISHOP:
        return 'b';
        break;
    case W_QUEEN:
        return 'Q';
        break;
    case B_QUEEN:
        return 'q';
        break;
    case W_KING:
        return 'K';
        break;
    case B_KING:
        return 'k';
        break;
    default:
        return '?';
        break;
    }
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
