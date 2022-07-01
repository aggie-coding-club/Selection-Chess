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
#include <Camera2D.hpp>

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
    Camera2D* camera;
    
    std::unique_ptr<Game> game;

    // Coords of the center chunk last frame.
    SignedCoords prevChunk;

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
    // WARNING: uses TileMaps' xy coordinates system. To convert to file-rank, negate _y value.
    void setCell(int _x, int _y, SquareEnum _squareVal, bool _isSelected=false, bool _isFloating=false, TileMapEnum _highlight=TM_EMPTY);

    // TODO: behavior of this around decorators, when to use, etc should be clarified
    void redrawBoard();

    // Get the chunk the camera is currently in
    SignedCoords getChunk();

    // Get integer TileMap coords from a global float position
    SignedCoords getCoordsFromGlobalPos(Vector2 _global);
};

}

#endif
