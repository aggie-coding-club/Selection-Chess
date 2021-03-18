#ifndef ENGINE_RUNNER_H
#define ENGINE_RUNNER_H

#include <cstdint>
#include <stack>

#include <boost/process.hpp> // note: requires boost >= 1_64_0
// Also note, if you are using VSCode, add your boost path as an includePath to your c_cpp_properties.json if your Intelisense isn't recognizing it
#include <boost/asio.hpp>

namespace bp = boost::process;
namespace asio = boost::asio;

#include "constants.h"
#include "board.h"
#include "tokenizer.h"

class Move {}; // FIXME: temporary until merged

class EngineRunner {
    protected:
        bp::opstream m_engineInputStream;
        boost::asio::io_service m_ios; // TODO: remove?
        bp::ipstream m_pipeStream;
        bool m_engineAlive;
        std::string m_engineName;

        // Cleans input by handling comments, messages, etc.
        // Throws exception if program closes prematurely
        Tokenizer* getCommand();

        // Process commands until we get the command type (namely, _cmdName) we want. 
        // If empty string given as _cmdName, all commands so far will be processed.
        // Set _ignore to consume commands but do nothing with them.
        Tokenizer* processCommands(std::string _cmdName="", bool _ignore=false);

    public:
        EngineRunner(std::string _path);

        void quit();

        Move getMove();

        bool setMove(Move _move);
};

#endif
