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

std::string PieceMove::algebraic() {
    return coordsToAlgebraic(m_startPos) + coordsToAlgebraic(m_endPos);
}
std::string TileMove::algebraic() {
    std::string result = "S" + coordsToAlgebraic(m_selFirst) + coordsToAlgebraic(m_selSecond) + signedCoordsToAlgebraic(m_translation);
    if (abs(m_symmetry) < 4) {
        result += "R" + std::to_string(abs(m_symmetry));
    }
    if (m_symmetry < 0) {
        result += "F";
    }
    return result;
}
std::string TileDeletion::algebraic() {
    std::string result = "D";
    for (Coords c : m_deleteCoords) {
        result += coordsToAlgebraic(c);
    }
    return result;
}

std::unique_ptr<Move> readAlgebraic(std::string _algebra) {
    dout << "readAlgebraic(" << _algebra << ")" << std::endl;
    AlgebraicTokenizer tokenizer(_algebra);

    if (isalpha(tokenizer.peek()[0]) && isupper(tokenizer.peek()[0])) { // got a capital prefix

        if (tokenizer.peek() == "S") { // this is a tile selection move
            tokenizer.next(); // eat the S
            Coords f = tokenizer.nextCoords(); Coords s = tokenizer.nextCoords(); Coords d = tokenizer.nextSignedCoords();
            std::unique_ptr<TileMove> move = std::make_unique<TileMove>(f, s, d);
            if (tokenizer.hasNext()) { // Using symmetry modifier(s)
                if (tokenizer.peek() == "R") { // Rotation
                    dout << "has rotation" << std::endl;
                    tokenizer.next(); // eat the R
                    int numRotations = std::stoi(tokenizer.next());
                    numRotations %= 4;
                    if (numRotations == 0) numRotations = 4;
                    move->m_symmetry = numRotations;
                }
                if (tokenizer.hasNext()) {
                    if (tokenizer.peek() == "F") { // reFlection/Flip. Note we don't care if we eat the F here since it is last character.
                        dout << "has flip" << std::endl;
                        move->m_symmetry *= -1;
                    } else {
                        dout << "UNKNOWN SUFFIX '" << tokenizer.peek() << "'" << WHERE << std::endl;
                    }
                }
            }
            return move;

        } else if (tokenizer.peek() == "D") { // this is a tile deletion move
            tokenizer.next(); // eat the D
            std::vector<Coords> deletions;
            while (tokenizer.hasNext()) {
                deletions.push_back(tokenizer.nextCoords());
            }
            return std::make_unique<TileDeletion>(deletions);

        } else {
            dout << "UNKNOWN PREFIX '" << tokenizer.peek() << "' " << WHERE << std::endl;
        }
    }
    // If no prefix, then this is a piece move
    Coords f = tokenizer.nextCoords(); Coords s = tokenizer.nextCoords(); // We declare them before passing as params to make sure order of operations is OK
    return std::make_unique<PieceMove>(f, s);
}

std::string coordsToAlgebraic(Coords _coords, Coords _offset) {
    //TODO: implement _offset if its needed
    return intToLetters(_coords.first) + std::to_string(_coords.second);
}
std::string signedCoordsToAlgebraic(SignedCoords _coords) {
    //TODO: implement _offset if its needed
    return (_coords.first  >= 0? "+" : "") + std::to_string(_coords.first) + 
           (_coords.second >= 0? "+" : "") + std::to_string(_coords.second);
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
        if (islower(lookahead)) { // lowercase letter is a file coord
            return matchHomogenous(m_stream, isalpha);
        }
        m_stream.ignore(); // pop character
        return std::string(1, lookahead);

    } else if (isdigit(lookahead)) { // digit is a rank coord
        return matchHomogenous(m_stream, isdigit);

    } else if (std::string("-+").find(lookahead) != std::string::npos) { // other permissible single-character lexemes
        m_stream.ignore(); // pop character
        return std::string(1, lookahead);

    } else {
        dout << WHERE << "Error: Unknown character in algebraic string '" << lookahead << "'" << std::endl;
        return "[ERROR!]";
    }
}
Coords AlgebraicTokenizer::nextCoords() {
    //TODO: handle errors
    std::string letters = next();
    std::string numbers = next();
    return std::make_pair(lettersToInt(letters), (unsigned int) std::stoi(numbers));
}
SignedCoords AlgebraicTokenizer::nextSignedCoords() {
    //TODO: handle errors
    std::string coordStr = next();
    coordStr += next();
    int firstCoords = std::stoi(coordStr);
    coordStr = next();
    coordStr += next();
    int secondCoords = std::stoi(coordStr);
    return std::make_pair(firstCoords, secondCoords);
}

