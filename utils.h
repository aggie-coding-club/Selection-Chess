#ifndef UTILITYMETHODS_H
#define UTILITYMETHODS_H

#include "constants.h"

#include <string>
#include <iostream>
#include <sstream>
#include <ios>
#include <stdlib.h>
#include <vector>

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

std::vector<std::string> split(const std::string str, const std::string regex_str);

// Guarantees positive result congruent to i % n, except in case of an overflow.
// See this post for more details: https://stackoverflow.com/questions/14997165/fastest-way-to-get-a-positive-modulo-in-c-c
inline int positiveModulo(int i, int n) {
    return (i % n + n) % n;
}

template <typename T, typename U>
inline std::ostream& operator<<(std::ostream& _stream, const std::pair<T,U>& _coords) {
    _stream << "(" << _coords.first << ", " << _coords.second << ")";
    return _stream;
}

// Define element-wise arithmetic on std::pairs. // TODO: probably should define a new pair object
template <typename T, typename U, typename X, typename Y>
std::pair<T,U>& operator+=(std::pair<T,U>& _lhs, const std::pair<X,Y>& _rhs) {
    _lhs.first += _rhs.first;
    _lhs.second += _rhs.second;
    return _lhs;
}
template <typename T, typename U, typename X, typename Y>
std::pair<T,U> operator+(std::pair<T,U> _lhs, const std::pair<X,Y>& _rhs) {
    return _lhs += _rhs;
}
template <typename T, typename U, typename X, typename Y>
std::pair<T,U>& operator-=(std::pair<T,U>& _lhs, const std::pair<X,Y>& _rhs) {
    _lhs.first -= _rhs.first;
    _lhs.second -= _rhs.second;
    return _lhs;
}
template <typename T, typename U, typename X, typename Y>
std::pair<T,U> operator-(std::pair<T,U> _lhs, const std::pair<X,Y>& _rhs) {
    return _lhs -= _rhs;
}

// Scalar multiplication on pairs
template <typename T, typename U, typename S>
std::pair<T,U>& operator*=(std::pair<T,U>& _pair, const S& _scalar) {
    _pair.first *= _scalar;
    _pair.second *= _scalar;
    return _pair;
}
template <typename T, typename U, typename S>
std::pair<T,U> operator*(std::pair<T,U> _pair, const S& _scalar) {
    return _pair *= _scalar;
}
template <typename T, typename U, typename S>
std::pair<T,U> operator*(const S& _scalar, std::pair<T,U> _pair) {
    return _pair *= _scalar;
}

// ModInt methods that are nice to do pairwise
// TODO: this gross syntax emphasizes why we need a custom pair class
template <typename T, typename U>
std::pair<T,U> getDistTo(const std::pair<T,U>& _lhs, const std::pair<T,U>& _rhs) {
    return std::make_pair(_lhs.first.getDistTo(_rhs.first), _lhs.second.getDistTo(_rhs.second));
}

// Comparators for pairs //TODO: remove if unneeded
// Row order implies f0 < a8
template <typename T, typename U>
struct comparePairRowOrder {
    bool operator()(const std::pair<T,U>& _a, const std::pair<T,U>& _b) const {
        if (_a.second != _b.second) {
            return _a.second < _b.second;
        } else {
            return _a.first < _b.first;
        }
    }
};
// Column Order implies a8 < f0
template <typename T, typename U>
struct comparePairColOrder {
    bool operator()(const std::pair<T,U>& _a, const std::pair<T,U>& _b) const {
        if (_a.first != _b.first) {
            return _a.first < _b.first;
        } else {
            return _a.second < _b.second;
        }
    }
};
// Comparators for pairs of modular ints -- this is needed for storing them in maps
// Row order implies f0 < a8
template <typename T, typename U>
struct compareModPairRowOrder {
    bool operator()(const std::pair<T,U>& _a, const std::pair<T,U>& _b) const {
        if (_a.second != _b.second) {
            return _a.second.m_value < _b.second.m_value;
        } else {
            return _a.first.m_value < _b.first.m_value;
        }
    }
};
// Column Order implies a8 < f0
template <typename T, typename U>
struct compareModPairColOrder {
    bool operator()(const std::pair<T,U>& _a, const std::pair<T,U>& _b) const {
        if (_a.first != _b.first) {
            return _a.first.m_value < _b.first.m_value;
        } else {
            return _a.second.m_value < _b.second.m_value;
        }
    }
};

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
    return spaceEnum != EMPTY && spaceEnum != INVALID; 
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
