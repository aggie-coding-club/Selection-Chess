#ifndef RULESET_H
#define RULESET_H

#include "constants.h"

#include <vector>
#include <iostream>

typedef int MOVEMENT_MODE;
enum : MOVEMENT_MODE {NORMAL_MOVEMENT, CAPTURE_ONLY, MOVE_ONLY, PROJECTILE};
typedef int MOVEOPTION_TYPE;
enum : MOVEMENT_MODE {};

// class MoveOption {
//     virtual getMoves() = 0;
// };

// class 

class Ruleset {
    public:
    // How many tiles can be deleted using TileDeletion in a single move
    int numDeletionsPerTurn = 1;
    // Allow TileMoves to rotate the moved piece
    bool allowRotations = false;
    // Allow TileMoves to mirror the moved piece
    bool allowReflections = false;

    std::vector<std::vector<MoveOption>> pieceMoveOptions;
    // TODO: maybe timing info can be put here too?
};

#endif