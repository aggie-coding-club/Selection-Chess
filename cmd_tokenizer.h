#ifndef CMD_TOKENIZER_H
#define CMD_TOKENIZER_H

#include "tokenizer.h"
#include "constants.h"

// Tokenizer for parsing commands sent to GUI from engine
class CmdTokenizer : public AbstractTokenizer {
    public:
        CmdTokenizer (std::string _string) : AbstractTokenizer(_string) {};
        std::string next();
};

// TODO: probably provide another tokenizer for commands from engine to GUI here. 
// It'll share much of the same code, so we can probably just put it in this file.

#endif