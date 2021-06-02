#ifndef CMD_TOKENIZER_H
#define CMD_TOKENIZER_H

#include "tokenizer.h"
#include "constants.h"

// Tokenizer for parsing commands sent to GUI from engine
class CmdTokenizer : public StringTokenizer {
    public:
        CmdTokenizer (std::string _string) : StringTokenizer(_string) {};
        std::string next();

    private:
        inline bool isWhitespaceSansNewline(char _c);
        inline void eatWhitespace(std::istream& _stream);
        bool isReservedLexemeChar(char _c);
        std::string matchQuotedToken(std::istream& _stream, char _delim, bool _includeDelim=true);
        std::string matchWordToken(std::istream& _stream);
};

// TODO: probably provide another tokenizer for commands from engine to GUI here. 
// It'll share much of the same code, so we can probably just put it in this file.

#endif