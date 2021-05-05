#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <sstream>

// Provides an interface for other tokenizers to use.
// A child of this class only has to implement next().
class AbstractTokenizer {
    protected:
        std::stringstream m_stream;
        std::string m_peeked;
        bool m_hasPeeked = false;
    public:
        AbstractTokenizer(std::string _string) : m_stream(_string) { };

        // gets the next lexeme and pops it
        virtual std::string next() = 0;

        // gets the next lexeme without popping it
        std::string peek() {
            if (!m_hasPeeked) {
                m_peeked = next();
                m_hasPeeked = true;
            }
            return m_peeked;
        }

        // returns if there are no more lexemes.
        // Override this if the provided code does not work for your tokenizer
        virtual bool hasNext() {
            return m_hasPeeked || m_stream.peek() != EOF;
        }
};

#endif