#ifndef UTILITYMETHODS_H
#define UTILITYMETHODS_H

#include <string>
#include <iostream>

#include "constants.h"
#include "board.h"

// This macro is how "dout << ..." works
extern std::ostream g_nullout;
#ifdef DEBUG
#define dout std::cout
#else
#define dout g_nullout
#endif

/**
 * Gets letter respresentation of piece type enum
 * Optionally set empty to desired character representation of EMPTY and INVALID
 */
char getCharFromPiece(PieceEnum _enumValue, char _empty=PIECE_LETTERS[EMPTY], char _invalid=PIECE_LETTERS[INVALID]);
/**
 * Gets piece enum from letter
 * Optionally set empty to desired character representation of EMPTY
 * Returns INVALID for unknown characters.
 */
PieceEnum getPieceFromChar(char _char, char _empty=PIECE_LETTERS[EMPTY]);

/**
 * Gets letter respresentation of direction
 */
char getCharFromDirection(DirectionEnum _dir);

/**
 * LEFT to RIGHT, etc.
 */
DirectionEnum flipDirection(DirectionEnum _dir);

/**
 * Gets unicode chess piece respresentation of piece type enum
 * OPTIONALLY set empty to desired character representation of EMPTY, 
 */
std::string getUnicodeCharFromEnum(PieceEnum _enumValue, std::string _empty=" ");

/** Is this a valid nonempty piece enum? */
inline bool isPiece(PieceEnum spaceEnum) {
    return spaceEnum != EMPTY; 
}
/** Assuming a valid nonempty piece enum, is the piece white or black? */
inline bool isWhite(PieceEnum piece) {
    return piece & 1u; // Gets the LSB
}
/** Assuming a valid nonempty piece enum, is the piece white or black? */
inline bool isBlack(PieceEnum piece) {
    return !isWhite(piece); // Assumes black pieces are even enums
}
/** Converts the piece to white, if not already. Note this also converts INVALID to EMPTY. */
inline PieceEnum toWhite(PieceEnum piece) {
    // just overwrite LSB.
    return piece | 1u; //Assumes white pieces are odd enums
}
/** Converts the piece to black, if not already. Note this also converts EMPTY to INVALID. */
inline PieceEnum toBlack(PieceEnum piece) {
    // just overwrite LSB.
    return piece & !1u; //Assumes black pieces are even enums
}
/** Converts the piece to non-colored enum, as used by Move. */
inline unsigned char uncolor(PieceEnum piece) {
    // just strip LSB.
    return piece >> 1;
}
/** Converts the piece from a non-colored enum to White. */
inline PieceEnum recolor(unsigned char piece) {
    // just add LSB of zero.
    return piece << 1;
}

#endif
