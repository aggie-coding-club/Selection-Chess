#include "gdn_utils.h"
#include "constants.hpp"

#include <cassert>
#include <string>
// #include <algorithm>

// TileMapEnum getTMFromSquare(SquareEnum _squareEnum) {
//     return SQUAREENUM_TO_TILEMAPENUM[_squareEnum];
// }

// SquareEnum getSquareFromTM(TileMapEnum _tmEnum) {
//     SquareEnum found = *std::find(SQUAREENUM_TO_TILEMAPENUM.begin(), SQUAREENUM_TO_TILEMAPENUM.end(), _tmEnum);
//     if (found == std::string::npos) {
//         return EMPTY;
//     }
//     return found;
// }

std::string variantToStdString(godot::Variant _var) {
    assert(_var.get_type() == godot::Variant::Type::STRING);
    // I don't know if there is a better way to convert these types...
    godot::String godotString = _var;
    char* godotAllocatedCString = godotString.alloc_c_string();
    std::string ret(godotAllocatedCString);
    godot::api->godot_free(godotAllocatedCString);
    return ret;
}
