#ifndef GODOT_CONSTANTS_HPP
#define GODOT_CONSTANTS_HPP

#include <string>
#include <vector>

// TileMap piece types. Note TM_EMPTY used for all TileMapEnums
typedef int TileMapEnum;
enum : TileMapEnum {TM_EMPTY=-1, TM_B_BISHOP=0, TM_B_KING, TM_B_KNIGHT, TM_B_PAWN, TM_B_QUEEN, TM_B_ROOK, TM_W_BISHOP, TM_W_KING, TM_W_KNIGHT, TM_W_PAWN, TM_W_QUEEN, TM_W_ROOK};
enum : TileMapEnum {TM_TILE=0, TM_TILE_HIGHLIGHTED};
enum : TileMapEnum {TM_HIGHLIGHT_DOT=0, TM_HIGHLIGHT_CIRCLE};

// typedef int SelectionState;
// enum : SelectionState {NO_SEL, PIECE_SEL, TILES_SEL};
// typedef int CursorMode;
// enum : CursorMode {PIECE_MODE, TILES_MODE, DELETE_MODE};
typedef enum {NO_SEL, PIECE_SEL, TILES_SEL} SelectionState;
typedef enum {PIECE_MODE, TILES_MODE, DELETE_MODE} CursorMode;

// Mirrors SquareEnum definition
const std::vector<TileMapEnum> SQUAREENUM_TO_TILEMAPENUM {TM_EMPTY, TM_W_PAWN, TM_B_PAWN, TM_W_ROOK, TM_B_ROOK, TM_W_KNIGHT, TM_B_KNIGHT, TM_W_BISHOP, TM_B_BISHOP, TM_W_QUEEN, TM_B_QUEEN, TM_W_KING, TM_B_KING}; 
#endif