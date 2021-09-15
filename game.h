#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "board.h"
#include "ruleset.h"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

class Game {
    public:
        Board* m_board = nullptr;
        // list of moves applied to starting SFEN.
        std::stack<std::shared_ptr<Move>> m_moveHistory; 

        // Number of full moves (increments after black plays).
        // WARNING: starts at 1.
        int m_moveCount = 1;

        // 50 move rule counter. Resets at zero anytime a pawn moves or a 
        // capture happens, stalemate happens if this reaches 50.
        short m_50Count = 0; 
        // whose turn it is
        PieceColor m_turn = WHITE;
        // What rules we are playing with
        Ruleset m_rules;

        // TODO: probably should be able to take in PGNs too.
        Game(std::string _sfen, std::string _rulesFile="default.rules"); // TODO: weird hardcode
        void reset(const std::string _sfen);

        bool applyMove(std::shared_ptr<Move> _move);

        bool undoMove(size_t _numMoves=1);

        uint64_t getHash() const {
            //TODO: implement
            return 0u;
        };

        std::string toSfen() const;

        std::string print();
        std::string printMoveHistory(size_t _numTurns);
};

#endif