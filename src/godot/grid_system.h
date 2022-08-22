#ifndef __GRID_SYSTEM_H__
#define __GRID_SYSTEM_H__

/** Defines the GDNative script that we attach to the GridSystem node in Godot. */

#include "../utils/utils.h"
#include "../chess/constants.hpp"
#include "../chess/game.h"
#include "constants.hpp"
#include "../interface/human_runner.h"
#ifndef NO_BOOST
    #include "../interface/engine_runner.h"
#endif

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

    std::unique_ptr<Game> m_game;
    std::unique_ptr<PlayerRunner> m_whiteEngine;
    std::unique_ptr<PlayerRunner> m_blackEngine;

    // Cache of its board's sfen.
    std::string m_sfen;

    TileMap* pieceTileMap;
    TileMap* boardTileMap;
    TileMap* highlightsTileMap;
    TileMap* pieceTileMapFloating;
    TileMap* boardTileMapFloating;
    Camera2D* camera;
    // Root of GameSpace scene
    Node2D* gameSpaceNode;
    
    // Coords of the center chunk last frame.
    SignedCoords prevChunk;

public:
    static void _register_methods();

    GridSystem();
    ~GridSystem();

    void _init(); // our initializer called by Godot

    void _process(float delta);
    void _ready();
    void _input(Variant event);

    void reset_sfen(Variant _sfen);
    void add_engine(Variant _path, PieceColor _player);
    // std::string get_sfen();

private:
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
