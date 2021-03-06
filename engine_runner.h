#ifndef ENGINE_RUNNER_H
#define ENGINE_RUNNER_H

#include "player_runner.h"
#include "cmd_tokenizer.h"
#include "constants.h"

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
        bp::opstream m_engineInputStream;
        boost::asio::io_service m_ios; // TODO: remove?
        bp::ipstream m_pipeStream;
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

        bool setMove(std::unique_ptr<Move>& _move);

        // fills m_cmdQueue as input comes in
        void run();
};

#endif
