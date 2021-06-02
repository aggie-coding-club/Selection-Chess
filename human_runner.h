#ifndef HUMAN_RUNNER_H
#define HUMAN_RUNNER_H

#include "player_runner.h"
#include "cmd_tokenizer.h"
#include "constants.h"

#include <cstdint>
#include <stack> // TODO: remove?
#include <queue>

class HumanRunner : public PlayerRunner {
    protected:
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
        HumanRunner(std::string _name);

        bool init();

        void quit();

        std::unique_ptr<Move> getMove();

        bool setMove(std::unique_ptr<Move>& _move);

        // fills m_cmdQueue as input comes in
        void run();
};

#endif
