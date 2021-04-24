#include "move.h"
#include "constants.h"
#include "utils.h"
#include <math.h>

// Note: all of the error checks to make sure the format is correct can be ommited by a chess engine when it communicates with the gui, if it wants to.

// ----------- Conversions for the letter lexeme of algebraic notation ----------- //
unsigned int lettersToInt(std::string _letters) {
    // Note that upper case letters are prohibited
    unsigned int value = 0;
    size_t place = 0; // which digit are we on?
    for (auto it = _letters.crbegin(); it != _letters.crend(); it++) {
        if (*it != 'z') {
            value += (*it - 'a' + 1) * pow(26, place);
        }
        place++;
    }
    return value;
}
std::string intToLetters(unsigned int _int) {
    std::string letters = "";
    if (_int == 0) return "a"; // weird edge case
    // First, get the starting place
    // size_t place = 0;
    unsigned int placeUnit;
    // FIXME: bad behavior on overflow
    for (placeUnit = 1; placeUnit < _int; placeUnit *= 26) {
        // place++;
    }
    placeUnit /= 26; // since we went over by one in the loop
    // Then, get letters
    for (; placeUnit > 0; placeUnit /= 26) {
        dout << "placeUnit is " << placeUnit << std::endl;
        unsigned int thisDigit = _int / placeUnit;
        dout << "thisDigit is " << thisDigit << std::endl;
        char thisDigitChar = (thisDigit == 0)? 'z' : thisDigit + 'a' - 1; 
        dout << "thisDigitChar is " << thisDigitChar << std::endl;
        letters += thisDigitChar;
        _int %= placeUnit;
        dout << "_int is " << _int << std::endl;
    }
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
