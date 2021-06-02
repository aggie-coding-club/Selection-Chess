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
        // list of moves applied to starting FEN.
        std::stack<std::shared_ptr<Move>> m_moveHistory; 

        // 50 move rule
        short m_movesSinceLastCapture = 0; 
        // whose turn it is
        PieceColor m_turn = WHITE;
        // What rules we are playing with
        Ruleset m_rules;
        
        explicit Game(const std::string _sfen);
        void reset(const std::string _sfen);

        bool applyMove(std::shared_ptr<Move> _move);

        bool undoMove(size_t _numMoves=1);

        uint64_t getHash() const {
            //TODO: implement
            return 0u;
        };

        std::string print();
        std::string Game::printMoveHistory(size_t _numTurns);
};

#endif