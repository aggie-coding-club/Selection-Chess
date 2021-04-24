#ifndef MOVE_H
#define MOVE_H

#include "constants.h"

#include <sstream>

class Move {
    public:
        Coords m_startPos;
        Coords m_endPos;
        Move (Coords _startPos, Coords _endPos) : m_startPos(_startPos), m_endPos(_endPos) { }
        Move () { }

        //TODO: standard algebraic notation

        std::string algebraic();
};

Move readAlgebraic(std::string _algebra);

std::string coordsToAlgebraic(Coords _coords, Coords _offset=std::make_pair(0,0));
Coords algebraicToCoords(std::string _algebra, Coords _offset=std::make_pair(0,0));

//TODO: move this class somewhere better, and make Tokenizer used in xboard commands a child of it
class AbstractTokenizer {
    protected:
        std::stringstream m_stream;
        std::string m_peeked;
        bool m_hasPeeked = false;
    public:
        AbstractTokenizer(std::string _string) : m_stream(_string) { };
        virtual std::string next() = 0;
        std::string peek();
};

class AlgebraicTokenizer : AbstractTokenizer {
    public:
        AlgebraicTokenizer(std::string _string) : AbstractTokenizer(_string) {}
        std::string next();
};


unsigned int lettersToInt(std::string _letters);
std::string intToLetters(unsigned int _int);

#endif