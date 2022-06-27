#include "gdn_utils.h"
#include "constants.hpp"

#include <string>
#include <algorithm>

TileMapEnum getTMFromSquare(SquareEnum _squareEnum) {
    return SQUAREENUM_TO_TILEMAPENUM[_squareEnum];
}

SquareEnum getSquareFromTM(TileMapEnum _tmEnum) {
    SquareEnum found = *std::find(SQUAREENUM_TO_TILEMAPENUM.begin(), SQUAREENUM_TO_TILEMAPENUM.end(), _tmEnum);
    if (found == std::string::npos) {
        return EMPTY;
    }
    return found;
}
