#ifndef CONSTANTS_H
#define CONSTANTS_H

#define STARTING_MATERIAL 0

typedef unsigned char PieceEnum;
/**
 * White is odd, black is even, excluding EMPTY=0.
 * We also use this so that this enum can be directly used as an array index for pieces
*/
enum : PieceEnum {EMPTY=0, W_PAWN, B_PAWN, W_ROOK, B_ROOK, W_KNIGHT, B_KNIGHT, W_BISHOP, B_BISHOP, W_QUEEN, B_QUEEN, W_KING, B_KING};

static const PieceEnum blackPieces[6] = {B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING};
static const PieceEnum whitePieces[6] = {W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING};

// piece value list:
// used as follows: PIECEVALUES[W_PAWN], for example
const short PIECEVALUES[] = {0, 100, -100, 500, -500, 300, -300, 300, -300, 900, -900, 10000, -10000};

static const char* EMPTY_BOARD_FEN = "8/8/8/8/8/8/8/8 w - - 0 1";

static const char* STARTING_BOARD_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

#endif