#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <sstream>
#include "constants.h"

class Tokenizer {
    protected:
        std::stringstream m_stream;
        std::string m_peeked;
        bool m_hasPeeked = false;
    public:
        Tokenizer (std::string _string);
        std::string next();
        std::string peek();
};


#endif