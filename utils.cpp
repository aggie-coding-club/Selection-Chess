#include "utils.h"
#include "constants.h"

#include <string>
#include <map>
#include <regex>

std::stringstream dlogStream;

std::vector<std::string> split(const std::string str, const std::string regex_str) {
    std::regex regexz(regex_str);
    std::sregex_token_iterator token_iter(str.begin(), str.end(), regexz, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> list;
    while (token_iter != end) {
        list.emplace_back(*token_iter++);
    }
    return list;
}


DirectionEnum flipDirection(DirectionEnum _dir) {
    if (_dir & 1) { // if odd
        return _dir - 1;
    } else {
        return _dir + 1;
    }
}

char getCharFromSquare(SquareEnum _enumValue, char _empty, char _invalid) {
    if (_enumValue == EMPTY) {
        return _empty;
    }
    if (_enumValue == VOID) {
        return _invalid;
    }
    return TILE_LETTERS[_enumValue];
}

SquareEnum getSquareFromChar(char _char, char _empty) {
    if (_char == _empty) {
        return EMPTY;
    }
    std::size_t found = TILE_LETTERS.find(_char);
    if (found == std::string::npos) {
        return VOID;
    }
    return found;
}

std::string getUnicodeCharFromSquare(SquareEnum _enumValue, std::string _empty) {
    // This could be put into constants.h?
    std::map<SquareEnum, std::string> unicodePieceChars = {
        {EMPTY, _empty},
        {W_PAWN, "♙"},
        {B_PAWN, "♟︎"},
        {W_ROOK, "♖"},
        {B_ROOK, "♜"},
        {W_KNIGHT, "♘"},
        {B_KNIGHT, "♞"},
        {W_BISHOP, "♗"},
        {B_BISHOP, "♝"},
        {W_QUEEN, "♕"},
        {B_QUEEN, "♛"},
        {W_KING, "♔"},
        {B_KING, "♚"},
        {VOID, "?"} // not sure how this is intended to work
    };
    if (unicodePieceChars.count(_enumValue) > 0) {
        return unicodePieceChars[_enumValue];
    }
    return "?";
}

char getCharFromDirection(DirectionEnum _dir) {
    std::map<DirectionEnum, char> directionChars = {
        {LEFT, 'L'},
        {RIGHT, 'R'},
        {UP, 'U'},
        {DOWN, 'D'},
        {DOWN_LEFT, 'l'},
        {DOWN_RIGHT, 'r'},
        {UP_LEFT, '\\'},
        {UP_RIGHT, '/'}
    };
    if (directionChars.count(_dir) > 0) {
        return directionChars[_dir];
    }
    return '?';
}
