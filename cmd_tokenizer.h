#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "constants.h"

#include <sstream>

class CmdTokenizer {
    protected:
        std::stringstream m_stream;
        std::string m_peeked;
        bool m_hasPeeked = false;
    public:
        CmdTokenizer (std::string _string);
        std::string next();
        std::string peek();
};


#endif