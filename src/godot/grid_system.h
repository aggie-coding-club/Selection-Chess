#ifndef __GRID_SYSTEM_H__
#define __GRID_SYSTEM_H__

/** Defines the GDNative script that we attach to the GridSystem node in Godot. */

#include "../utils/utils.h"
#include "../chess/constants.hpp"
#include "../chess/game.h"
#include "constants.hpp"

#include <Godot.hpp>
#include <Sprite.hpp>
#include <Node2D.hpp>
#include <TileMap.hpp>

namespace godot {

class GridSystem : public Node2D {
    GODOT_CLASS(GridSystem, Node2D)

private:
    float time_passed;
    float time_emit;
    float amplitude;
    float speed;

    TileMap* pieceTileMap;
    TileMap* boardTileMap;
    TileMap* highlightsTileMap;
    TileMap* pieceTileMapFloating;
    TileMap* boardTileMapFloating;

    std::unique_ptr<Game> game;

public:
    static void _register_methods();

    GridSystem();
    ~GridSystem();

    void _init(); // our initializer called by Godot

    void _process(float delta);
    void _ready();
    void set_speed(float p_speed);
    float get_speed();

    // Calls set_cell on Godot boards to change the displayed cell.
    // Use _isSelected, _isFloating, and _highlight to change decorators.
    // Valid values for _highlight are TM_HIGHLIGHT_DOT or TM_HIGHLIGHT_CIRCLE.
    // Behavior for invalid combinations of decorators is undefined.
    void set_cell(int _x, int _y, SquareEnum _squareVal, bool _isSelected=false, bool _isFloating=false, TileMapEnum _highlight=TM_EMPTY);

    void redrawBoard();
};

}

#endif
