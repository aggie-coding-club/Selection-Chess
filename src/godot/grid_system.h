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
};

}

#endif
