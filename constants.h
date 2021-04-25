#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#define STARTING_MATERIAL 0 //FIXME: make constant type

typedef std::pair<unsigned int, unsigned int> Coords;

typedef int CustomException;
enum : CustomException {EXCP_ENGINE_DIED=101};

/**
 * White is odd, black is even, excluding EMPTY=0 and INVALID.
 * We also use this so that this enum can be directly used as an array index for pieces
*/
typedef unsigned char PieceEnum;
enum : PieceEnum {EMPTY=0, W_PAWN=1, B_PAWN, W_ROOK, B_ROOK, W_KNIGHT, B_KNIGHT, W_BISHOP, B_BISHOP, W_QUEEN, B_QUEEN, W_KING, B_KING, INVALID};

typedef unsigned char PieceType;
enum : PieceType {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};

typedef unsigned char PieceColor;
enum : PieceColor {BLACK, WHITE};
// parallel to piece enum. This is how we read SFEN
const std::string PIECE_LETTERS = ".PpRrNnBbQqKk?";

/**
 * Paired up so opposites are next to eachother and can be easily flipped.
 * Sometimes we only use the first 4 as indices, which is why they are first.
*/
typedef unsigned char DirectionEnum;
enum : DirectionEnum {LEFT, RIGHT, UP, DOWN, DOWN_LEFT, UP_RIGHT, DOWN_RIGHT, UP_LEFT};

const int NUM_PIECE_TYPES = 6;
static const PieceEnum BLACK_PIECES[NUM_PIECE_TYPES] = {B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING};
static const PieceEnum WHITE_PIECES[NUM_PIECE_TYPES] = {W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING};

// piece value list: //FIXME: this is very limited way to look at piece value
// used as follows: PIECE_VALUES[W_PAWN], for example
const short PIECE_VALUES[] = {0, 100, -100, 500, -500, 300, -300, 300, -300, 900, -900, 10000, -10000};

static const char* EMPTY_BOARD_FEN = "8/8/8/8/8/8/8/8 w - - 0 1";

static const char* TRADITIONAL_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static const char* ASCII_LOGO =
"      ______   ______   __                       \n"
"     /\\  ___\\ /\\  ___\\ /\\ \\                      \n"
"     \\ \\___  \\\\ \\  __\\ \\ \\ \\____                 \n"
"      \\/\\_____\\\\ \\_____\\\\ \\_____\\                \n"
"       \\/_____/ \\/_____/ \\/_____/                \n"
"   ______   __  __   ______   ______   ______    \n"
"  /\\  ___\\ /\\ \\_\\ \\ /\\  ___\\ /\\  ___\\ /\\  ___\\   \n"
"  \\ \\ \\____\\ \\  __ \\\\ \\  __\\ \\ \\___  \\\\ \\___  \\  \n"
"   \\ \\_____\\\\ \\_\\ \\_\\\\ \\_____\\\\/\\_____\\\\/\\_____\\ \n"
"    \\/_____/ \\/_/\\/_/ \\/_____/ \\/_____/ \\/_____/ \n"
"   ================================================\n";
#endif