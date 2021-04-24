#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "board.h"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

class Game {
    public:
        Board* m_board;
        std::stack<Move> m_moveHistory; // list of moves applied to starting FEN.

        short m_movesSinceLastCapture = 0; // 50 move rule
        bool m_turnWhite = true; // whose turn it is
        
        /** 
         * Creates a new board from SFEN.
         */
        Game(const std::string _sfen);

        bool applyMove(Move _move);

        bool undoMove(size_t _numMoves=1);

        uint64_t getHash() const {
            //TODO: implement
            return 0u;
        };

        std::string print();
        std::string Game::printMoveHistory(size_t _numTurns);
};

#endif