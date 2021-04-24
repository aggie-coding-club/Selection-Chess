#include "move.h"
#include "constants.h"
#include "utils.h"

#include <math.h>

// Note: all of the error checks to make sure the format is correct can be ommited by a chess engine when it communicates with the gui, if it wants to.

// ----------- Conversions for the letter lexeme of algebraic notation ----------- //
// a=0, ..., z=25, aa=26+0, ..., az=26+25, ba=2*26+0, ...
// Note that upper case letters are prohibited
unsigned int lettersToInt(std::string _letters) {
    if (_letters.length() > 2) {
        dout << "ERROR: coords too big. " << WHERE << std::endl;
        return -1;
    }
    unsigned int value = 0;

    if (_letters.length() == 2) {
        value += (_letters.at(0)-'a'+1) * 26;
        value += _letters.at(1)-'a';
    } else { // length=1
        value += _letters.at(0)-'a';
    }
    return value;
}
std::string intToLetters(unsigned int _int) {
    std::string letters = "";
    if (_int >= 26) {
        letters += (_int/26)+'a'-1;
    }
    letters += (_int%26)+'a';
    return letters;
}

std::string Move::algebraic() {
    return "";
}

Move readLongAlgebraic(std::string _algebra) {
    AlgebraicTokenizer tokenizer(_algebra);
    // TODO: handle errors
    return Move(
        std::make_pair(
            lettersToInt(tokenizer.next()),
            std::stoi(tokenizer.next())
        ),
        std::make_pair(
            lettersToInt(tokenizer.next()),
            std::stoi(tokenizer.next())
        )
    );
}

std::string coordsToAlgebraic(Coords _coords, Coords _offset) {
    //TODO: implement _offset if its needed
    return intToLetters(_coords.first) + std::to_string(_coords.second);
}

Coords algebraicToCoords(std::string _algebra, Coords _offset) {
    //TODO: implement _offset if its needed
    //TODO: handle errors
    AlgebraicTokenizer tokenizer(_algebra);
    std::string letterPart = tokenizer.next();
    std::string numberPart = tokenizer.next();
    return std::make_pair(
        lettersToInt(letterPart),
        std::stoi(numberPart)
    );
}

std::string matchHomogenous(std::istream& _stream, int (*_homoFunc)(int)) {
    std::string token = "";
    for(;;) {
        char lookahead = _stream.peek();

        if (!_homoFunc(lookahead)) {
            return token;
        }
        token += lookahead;
        _stream.ignore(); // pop character
    }
}

std::string AlgebraicTokenizer::next() {
    if (m_hasPeeked) {
        m_hasPeeked = false;
        return m_peeked;
    }

    char lookahead = m_stream.peek();
    if (isalpha(lookahead)) {
        return matchHomogenous(m_stream, isalpha);
    } else if (isdigit(lookahead)) {
        return matchHomogenous(m_stream, isdigit);
    } else {
        dout << WHERE << "Error: Unknown character in algebraic string '" << lookahead << "'" << std::endl;
        return "[ERROR!]";
    }
}

std::string AbstractTokenizer::peek() {
    if (!m_hasPeeked) {
        m_peeked = next();
        m_hasPeeked = true;
    }
    return m_peeked;
}
