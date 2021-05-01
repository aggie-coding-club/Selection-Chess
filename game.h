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
        Board* m_board = nullptr;
        std::stack<std::shared_ptr<Move>> m_moveHistory; // list of moves applied to starting FEN.

        short m_movesSinceLastCapture = 0; // 50 move rule
        PieceColor m_turn = WHITE; // whose turn it is
        
        /** 
         * Creates a new board from SFEN.
         */
        Game(const std::string _sfen);
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