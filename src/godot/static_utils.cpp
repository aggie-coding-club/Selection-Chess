#include "static_utils.h"
#include "gdn_utils.h"
#include "../chess/game.h"

#include <Viewport.hpp>

using namespace godot;

void StaticUtils::_register_methods() {
    register_method("coords_to_algebraic", &StaticUtils::coords_to_algebraic);
    register_method("algebraic_to_coords", &StaticUtils::algebraic_to_coords);
    register_method("letters_to_int", &StaticUtils::letters_to_int);
    register_method("int_to_letters", &StaticUtils::int_to_letters);
}

StaticUtils::StaticUtils() {
}

StaticUtils::~StaticUtils() {
    // add your cleanup here
}

void StaticUtils::_init() {
    // initialize any variables here.
}

String StaticUtils::coords_to_algebraic(int _file, int _rank) {
    return coordsToAlgebraic(DModCoords(_file, _rank)).c_str();
}

Array StaticUtils::algebraic_to_coords(String _algebra) {
    std::string algebra = variantToStdString(_algebra);
    DModCoords coords = algebraicToCoords(algebra);
    return Array::make(coords.file.m_value, coords.rank.m_value);
}

int StaticUtils::letters_to_int(String _letters) {
    std::string letters = variantToStdString(_letters);
    return lettersToInt(letters).m_value;
}

String StaticUtils::int_to_letters(int _file) {
    return intToLetters(DAModInt(_file)).c_str();
}