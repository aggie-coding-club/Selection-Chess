#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "board.h"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

class GameState {
    public:
        short m_movesSinceLastCapture; // 50 move rule
        bool m_turnWhite; // whose turn it is
        // std::stack<Move> moveHistory; // list of moves applied to starting FEN.

        /* ------- dependent fields, store information about board that is derived from independent fields -------- */
        short m_material; // changed material score to just be material for both
};

class Game {
    public:
        Board* m_board;
        
        /** 
         * Creates a new board from SFEN.
         */
        Game(const std::string _sfen);

        bool moveSelection(Coords _select1, Coords _select2, Coords _goal1) {
            //TODO: implement
            return false;
        };

        bool movePiece(Coords _start, Coords _goal) {
            //TODO: implement
            return false;
        };

        bool undo(size_t _numMoves=1) {
            //TODO: implement
            return false;
        };

        uint64_t getHash() const {
            //TODO: implement
            return 0u;
        };

        std::string print();

};

#endif