#ifndef ENGINE_RUNNER_H
#define ENGINE_RUNNER_H

#include "player_runner.h"
#include "../chess/cmd_tokenizer.h"
#include "../chess/constants.hpp"

#include <cstdint>
#include <stack> // TODO: remove?
#include <queue>

#include <boost/process.hpp> // note: requires boost >= 1_64_0
// Also note, if you are using VSCode, add your boost path as an includePath to your c_cpp_properties.json if your Intelisense isn't recognizing it
#include <boost/asio.hpp>

namespace bp = boost::process;
namespace asio = boost::asio;

class EngineRunner : public PlayerRunner {
    protected:
        // Send commands to the engine's std::cin using this
        bp::opstream m_toEngineStream;
        // Recieve commands/info from the engine's std::cout using this
        bp::ipstream m_fromEngineStream;
        std::queue<std::string> m_cmdQueue;
        std::string m_enginePath;

        // Cleans input by handling comments, messages, etc.
        // Throws exception if program closes prematurely
        CmdTokenizer* getCommand();

        // Process commands until we get the command type (namely, _cmdName) we want. 
        // If empty string given as _cmdName, all commands so far will be processed.
        // Set _ignore to consume commands but do nothing with them.
        CmdTokenizer* processCommands(std::string _cmdName="", bool _ignore=false);

    public:
        EngineRunner() = default;
        EngineRunner(std::string _path);

        bool init(std::string _path);
        bool init();

        void quit();

        std::unique_ptr<Move> getMove();

        bool setMove(const Move& _move);

        // fills m_cmdQueue as input comes in
        void run();

        bool setBoard(std::string _sfen);

};

#endif
