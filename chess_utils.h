#ifndef CHESS_UTILITYMETHODS_H
#define CHESS_UTILITYMETHODS_H

#include "constants.h"
#include "utils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <ios>
#include <stdlib.h>
#include <vector>
#include <regex>

// TODO: might be good to wrap some of these functions in a namespace like "SCUtils::" or smth

// ----------- SquareEnum functions ----------- //
/**
 * Gets letter respresentation of square type enum
 * Optionally set empty to desired character representation of EMPTY and VOID
 */
char getCharFromSquare(SquareEnum _enumValue, char _empty=TILE_LETTERS[EMPTY], char _invalid=TILE_LETTERS[VOID]);
/**
 * Gets square enum from letter
 * Optionally set empty to desired character representation of EMPTY
 * Returns VOID for unknown characters.
 */
SquareEnum getSquareFromChar(char _char, char _empty=TILE_LETTERS[EMPTY]);

/**
 * Gets unicode chess piece respresentation of square type enum
 * OPTIONALLY set empty to desired character representation of EMPTY, 
 */
std::string getUnicodeCharFromSquare(SquareEnum _enumValue, std::string _empty=" ");

/** Is this a piece enum? */
inline bool isPiece(SquareEnum _squareEnum) {
    return _squareEnum != EMPTY && _squareEnum != VOID; 
}
/** Assuming a valid piece enum, is the piece white or black? */
inline bool isWhite(SquareEnum _piece) {
    return _piece & 1u; // Gets the LSB
}
/** Assuming a valid nonempty piece enum, is the piece white or black? */
inline bool isBlack(SquareEnum _piece) {
    return !isWhite(_piece); // Assumes black pieces are even enums
}

// ----------- DirectionEnum functions ----------- //

/**
 * Gets letter respresentation of direction
 */
char getCharFromDirection(DirectionEnum _dir);

/**
 * LEFT to RIGHT, etc.
 */
DirectionEnum flipDirection(DirectionEnum _dir);

//----------- Here are some function useful for coordinate wrapping ----------//

// Compares which coords is more to the left or down from another coord, based on where the wrap-around happens (namely, at _relZero)
// returns if _lhs < _rhs
// TODO: consider merging with modularInt's methods
// It seems this is only used by DLLBoard, so i'd consider this to be deprecated
inline bool coordLessThan(unsigned int _lhs, unsigned int _rhs, unsigned int _relZero) {
    return (_lhs - _relZero) < (_rhs - _relZero);
}
inline bool coordGreaterThan(unsigned int _lhs, unsigned int _rhs, unsigned int _relZero) {
    return (_lhs - _relZero) > (_rhs - _relZero);
}

// Gets the distance from one coord to another, without crossing the wrap around point _relZero
inline unsigned int coordDistance(unsigned int _1, unsigned int _2, unsigned int _relZero) {
    if (coordLessThan(_1, _2, _relZero)) {
        return _2 - _1;
    } else {
        return _1 - _2;
    }
}

#endif
