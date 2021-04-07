#ifndef PLAYER_RUNNER_H
#define PLAYER_RUNNER_H

#include "constants.h"
#include "board.h"

class Move {}; // FIXME: temporary until merged

class PlayerRunner {
    protected:
        bool m_alive;
        std::string m_name;

    public:
        virtual bool init() = 0;

        virtual void quit() = 0;

        virtual Move getMove() = 0;

        virtual bool setMove(Move _move) = 0;

        virtual void run() = 0;
};

#endif
