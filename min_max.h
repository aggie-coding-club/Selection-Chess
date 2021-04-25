#ifndef MIN_MAX_H
#define MIN_MAX_H

#include "constants.h"
#include "game.h"

#include <limits>

// Minmax, meant to be easier to understand. Use the negamax implementation instead for actual usage.
// Generates a representation of the game tree and appends it to _history
std::pair<int,Move> minmax(Game* _game, int _depth, std::string& _history);

// Returns the benefit of the current position to the current player, and the best move.
// E.g. if the position is valued at -100 centipawns and its blacks turn, 
// it returns 100 centipawns plus the move that leads down that branch of the tree whose leaf has static eval of -100.
std::pair<int,Move> negamax(Game* _game, int _depth);

// Like negamax, but uses alpha-beta pruning.
std::pair<int,Move> negamaxAB(Game* _game, int _depth, 
    int _alpha=std::numeric_limits<int>::min()+1, // Note, we do the limit +- 1 to avoid any unexpected binary behavior. complement behavior
    int _beta=std::numeric_limits<int>::max()-1
); 

#endif