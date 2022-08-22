#ifndef GDN_UTILITYMETHODS_H
#define GDN_UTILITYMETHODS_H

#include "constants.hpp"
#include "../chess/constants.hpp"
#include <Godot.hpp>

// Converts a String-type Variant to a std::string.
std::string variantToStdString(godot::Variant _var);

// ----------- SquareEnum functions ----------- //
// Conversion to Godot's PieceTileMap
TileMapEnum getTMFromSquare(SquareEnum _squareEnum);
// Conversion from Godot's PieceTileMap
SquareEnum getSquareFromTM(TileMapEnum _tmEnum);

#endif
