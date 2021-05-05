#include "cmd_tokenizer.h"
#include "constants.h"
#include "utils.h"

// Note: since CmdTokenizer is only being called on strings created from std::getline(), 
// then anything here relating to '\n' is unecessary complexity.
// TODO: if this newline complexity is not used in the future, 
// we should probably remove it.

inline bool isWhitespaceSansNewline(char _c) {
    return (iswspace(_c) && _c != '\n');
}

inline void eatWhitespace(std::istream& _stream) {
  while(isWhitespaceSansNewline(_stream.peek())){ 
    _stream.ignore();
  }
}

// xboard word lexemes cannot contain these characters
bool isReservedLexemeChar(char _c) {
    const std::string RESERVED = "(){}\"=#\n";
    return std::string::npos != RESERVED.find(_c);
}

std::string matchQuotedToken(std::istream& _stream, char _delim, bool _includeDelim=true) {
    char firstDelim = _stream.get();
    std::string token(1, firstDelim);
    for(;;) {
        char lookahead = _stream.peek();

        if (!_includeDelim && lookahead == _delim) {
            return token;
        }
        if (lookahead == '\n' || lookahead == EOF) {
            dout << "Error with command: unmatched " << _delim << std::endl;
            return "[ERROR]"; // TODO:
        }
        token += lookahead;
        _stream.ignore(); // pop character

        if (lookahead == _delim) {
            return token;
        }
    }
}

std::string matchWordToken(std::istream& _stream) {
    std::string token = "";
    for(;;) {
        char lookahead = _stream.peek();
        if (iswspace(lookahead) || isReservedLexemeChar(lookahead) || lookahead == EOF) {
            return token;
        }
        token += lookahead;
        _stream.ignore(); // pop character
    }
}

CmdTokenizer::CmdTokenizer(std::string _string) : m_stream(_string) { }

std::string CmdTokenizer::next() {
    if (m_hasPeeked) {
        m_hasPeeked = false;
        return m_peeked;
    }
    eatWhitespace(m_stream); // eat leading whitespace

    char lookahead = m_stream.peek();

    // one-character lexemes
    if (lookahead == EOF || lookahead == ':' || lookahead == '\n' || lookahead == '=') {
        m_stream.ignore(); // pop character
        return std::string(1, lookahead);

    // quoted lexemes
    } else if (lookahead == '"') {
        return matchQuotedToken(m_stream, '"');
    } else if (lookahead == '(') {
        return matchQuotedToken(m_stream, ')');
    } else if (lookahead == '{') {
        return matchQuotedToken(m_stream, '}');
    } else if (lookahead == '#') {
        return matchQuotedToken(m_stream, EOF, false); //TODO: assumes \n never appears in string. This is ok assumption for now, but might be dangerous later. See Note above.

    // word lexemes aka miscellaneous
    } else {
        return matchWordToken(m_stream);
    }
}

std::string CmdTokenizer::peek() {
    if (!m_hasPeeked) {
        m_peeked = next();
        m_hasPeeked = true;
    }
    return m_peeked;
}
