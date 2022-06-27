#ifndef GODOT_CONSTANTS_HPP
#define GODOT_CONSTANTS_HPP

#include <string>
#include <vector>
#include "../utils/coords.hpp"

// TileMap piece types. Note TM_EMPTY used for all TileMapEnums
typedef int TileMapEnum;
enum : int {TM_EMPTY=-1, TM_B_BISHOP=0, TM_B_KING, TM_B_KNIGHT, TM_B_PAWN, TM_B_QUEEN, TM_B_ROOK, TM_W_BISHOP, TM_W_KING, TM_W_KNIGHT, TM_W_PAWN, TM_W_QUEEN, TM_W_ROOK};
enum : int {TM_TILE=0, TM_TILE_HIGHLIGHTED};
enum : int {TM_HIGHLIGHT_DOT=0, TM_HIGHLIGHT_CIRCLE};

// Mirrors SquareEnum definition
const std::vector<TileMapEnum> SQUAREENUM_TO_TILEMAPENUM {TM_EMPTY, TM_W_PAWN, TM_B_PAWN, TM_W_ROOK, TM_B_ROOK, TM_W_KNIGHT, TM_B_KNIGHT, TM_W_BISHOP, TM_B_BISHOP, TM_W_QUEEN, TM_B_QUEEN, TM_W_KING, TM_B_KING}; 
#endif