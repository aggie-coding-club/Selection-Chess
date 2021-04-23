#ifndef UTILITYMETHODS_H
#define UTILITYMETHODS_H

#include <string>
#include <iostream>
#include <sstream>
#include <ios>
#include <stdlib.h>

#include "constants.h"
#include "board.h"

// ----------- debugging macros ----------- //
// dout macro 'debug output'
// like cout, cerr, etc., except only outputs to console/log in debug mode.
extern std::ostream g_nullout;
#ifdef DEBUG
    #if DEBUG & (1<<0)
        #define dout std::cout
    #else 
        #define dout g_nullout
    #endif
    #if DEBUG & (1<<1)
        #define tdout std::cout
    #else 
        #define tdout g_nullout
    #endif
#else
    #define dout g_nullout
    #define tdout g_nullout
#endif

// USAGE: FORMAT(foo << bar << ... << baz) returns a std::string
// This macro lets us make std::string analogous to
// std::cout << foo << bar << baz
// Credit to Mr.Ree https://stackoverflow.com/questions/303562/c-format-macro-inline-ostringstream
#define FORMAT(ITEMS) \
    ( ( dynamic_cast<std::ostringstream &> ( \
        std::ostringstream() . std::ostream::seekp( 0, std::ios_base::cur ) << ITEMS ) \
    ) . str() )

// USAGE: returns a string describing where in the source code this macro is.
#define WHERE (FORMAT("\tFile: " << __FILE__ << "\n\tLine: " << __LINE__ << std::endl))

// ----------- PieceEnum functions ----------- //

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

// ----------- DirectionEnum functions ----------- //

/**
 * Gets letter respresentation of direction
 */
char getCharFromDirection(DirectionEnum _dir);

/**
 * LEFT to RIGHT, etc.
 */
DirectionEnum flipDirection(DirectionEnum _dir);

// ----------- operators on coords ----------- //

inline coords operator+(const coords & _l,const coords & _r) {   
    return std::make_pair(_l.first + _r.first, _l.second + _r.second);                                    
} 

inline coords operator-(const coords & _l,const coords & _r) {   
    return std::make_pair(_l.first - _r.first, _l.second - _r.second);                                    
} 

inline std::ostream& operator<<(std::ostream& _stream, const coords& _coords) {
    _stream << "(" << _coords.first << ", " << _coords.second << ")";
    return _stream;
}

#endif
