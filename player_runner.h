#ifndef PLAYER_RUNNER_H
#define PLAYER_RUNNER_H

#include "constants.h"
#include "board.h"
#include "move.h"

class PlayerRunner {
    protected:
        bool m_alive;
        std::string m_name;

    public:
        virtual bool init() = 0;

        virtual void quit() = 0;

        virtual std::unique_ptr<Move> getMove() = 0;

        virtual bool setMove(std::shared_ptr<Move>& _move) = 0;

        virtual bool setBoard(std::string _fen) = 0;

        virtual void run() = 0;
};

#endif
