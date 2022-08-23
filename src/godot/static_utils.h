#ifndef __STATIC_UTILS_FOR_GODOT_H__
#define __STATIC_UTILS_FOR_GODOT_H__

/** Defines the GDNative script as a Resource, which we can preload. */

#include "../utils/utils.h"
#include "../chess/constants.hpp"
#include "../chess/move.h"
#include "gdn_utils.h"
// #include "../chess/game.h"

#include <Godot.hpp>
#include <Resource.hpp>
#include <Array.hpp>
#include <String.hpp>

namespace godot {

class StaticUtils : public Resource {
    GODOT_CLASS(StaticUtils, Resource)

private:
    // This is a stateless object, so there are no member variables.
public:
    static void _register_methods();

    StaticUtils();
    ~StaticUtils();

    // CAUTION: python_case names refer to the functions we are exporting to Godot, while
    // camelCase names refer to the functions in the cpp source we are forwarding.
    // Therefore, this file should only be included by gdlibrary.cpp (to avoid using the wrong one)

    // Returns String of letters and numbers, as the algebraic form of position.
    String coords_to_algebraic(int _file, int _rank);
    // Returns an array integers of the form (x, y) corresponding to file, rank, given String _algebra in algebraic form
    Array algebraic_to_coords(String _algebra);
    // Returns numeric value of file, given its String _letters in algebraic form
    int letters_to_int(String _letters);
    // Returns String of letters in algebraic form, given its int value
    String int_to_letters(int _file);

    // TODO: consider changing all these "Variant" types to their more explicit types in the rest of the source

    void _init(); // our initializer called by Godot

};

}

#endif
