#include "grid_system.h"
#include "gdn_utils.h"
#include "../chess/game.h"

#include <Camera2D.hpp>
#include <Viewport.hpp>

using namespace godot;

void GridSystem::_register_methods() {
    register_method("_process", &GridSystem::_process);
    register_method("_ready", &GridSystem::_ready);
    // register_property<GridSystem, float>("amplitude", &GridSystem::amplitude, 10.0);
    // register_property<GridSystem, float>("speed", &GridSystem::set_speed, &GridSystem::get_speed, 1.0);

    register_signal<GridSystem>((char *)"position_changed", "node", GODOT_VARIANT_TYPE_OBJECT, "new_pos", GODOT_VARIANT_TYPE_VECTOR2);
}

GridSystem::GridSystem() {
}

GridSystem::~GridSystem() {
    // add your cleanup here
}

void GridSystem::_init() {
    // initialize any variables here
    time_passed = 0.0;
    amplitude = 10.0;
    speed = 1.0;
}

void GridSystem::_ready() {
    TileMap* pieceTileMap = (TileMap*) get_node("NodeBoard/PieceTileMap");
    TileMap* boardTileMap = (TileMap*) get_node("NodeBoard/BoardTileMap");
    TileMap* highlightsTileMap = (TileMap*) get_node("NodeBoard/HighlightsTileMap");
    TileMap* pieceTileMapFloating = (TileMap*) get_node("FloatingNodeBoard/PieceTileMap");
    TileMap* boardTileMapFloating = (TileMap*) get_node("FloatingNodeBoard/BoardTileMap");

    Camera2D* camera = (Camera2D*) get_node("../Camera2D");

    pieceTileMap->clear();
    boardTileMap->clear();
    highlightsTileMap->clear();
    pieceTileMapFloating->clear();
    boardTileMapFloating->clear();

    game = std::make_unique<Game>("P2R(2)p/1p3NR/BQp1/(3)1p w 0 1", "rules/piecesOnly.rules");

    dlog(game->print());

    for (size_t i = 0; i < 10; i++) for (size_t j = 0; j < 10; j++) {
        if ((i+j) % 2 == 0) {
            boardTileMap->set_cell(i, j, TM_TILE);
            pieceTileMap->set_cell(i, j, TM_B_QUEEN);
            if ((i+j) % 3 == 0) highlightsTileMap->set_cell(i, j, TM_HIGHLIGHT_CIRCLE);
        } else {
            boardTileMapFloating->set_cell(i, j, TM_TILE_HIGHLIGHTED);
            pieceTileMapFloating->set_cell(i, j, TM_B_QUEEN);
        }
    }
}

void GridSystem::set_cell(int _x, int _y, SquareEnum _squareVal, bool _isSelected, bool _isFloating, TileMapEnum _highlight) {
    if (_squareVal == VOID) {
        boardTileMap->set_cell(_x, _y, TM_EMPTY);
        pieceTileMap->set_cell(_x, _y, TM_EMPTY);
        boardTileMapFloating->set_cell(_x, _y, TM_EMPTY);
        pieceTileMapFloating->set_cell(_x, _y, TM_EMPTY);
        highlightsTileMap->set_cell(_x, _y, TM_EMPTY);

    } else if (_isFloating) {
        boardTileMap->set_cell(_x, _y, TM_EMPTY);
        pieceTileMap->set_cell(_x, _y, TM_EMPTY);
        boardTileMapFloating->set_cell(_x, _y, _isSelected? TM_TILE_HIGHLIGHTED : TM_TILE);
        pieceTileMapFloating->set_cell(_x, _y, _squareVal);
        highlightsTileMap->set_cell(_x, _y, TM_EMPTY); // cannot highlight floating tiles

    } else {
        boardTileMapFloating->set_cell(_x, _y, TM_EMPTY);
        pieceTileMapFloating->set_cell(_x, _y, TM_EMPTY);
        boardTileMap->set_cell(_x, _y, _isSelected? TM_TILE_HIGHLIGHTED : TM_TILE);
        pieceTileMap->set_cell(_x, _y, _squareVal);
        highlightsTileMap->set_cell(_x, _y, _highlight);
    }
}

void GridSystem::redrawBoard() {
    auto baseCoords = game->m_board->m_displayCoordsZero;
    StandardArray sa = game->m_board->standardArray();

    for (auto f = 0; f < sa.m_dimensions.file; ++f) {
        for (auto r = 0; r < sa.m_dimensions.rank; ++r) {
            // FIXME: design how TileMaps should wrap
            // set_cell((baseCoords.file + f).m_value )
        }
    }
}

void GridSystem::_process(float delta) {
    time_passed += speed * delta;

    Vector2 new_position = Vector2(
        amplitude + (amplitude * sin(time_passed * 2.0)),
        amplitude + (amplitude * cos(time_passed * 1.5))
    );

    set_position(new_position);

    time_emit += delta;
    if (time_emit > 1.0) {
        emit_signal("position_changed", this, new_position);

        time_emit = 0.0;
    }

    // FIXME left off with a bug here to go find a new debugger
    // Godot::Godot::get_viewport().get_mouse_position()
    // dlog(boardTileMap->world_to_map(boardTileMap->to_local(get_viewport()->get_mouse_position())).x);
    // get_viewport()->get_mouse_position();
    // boardTileMap->to_local(Vector2::ZERO);
    // boardTileMap->to_local(get_viewport()->get_mouse_position());
    // if (boardTileMap == nullptr) { dlog("ouch");}
    // boardTileMap->world_to_map(boardTileMap->to_local(get_viewport()->get_mouse_position()));
}

void GridSystem::set_speed(float p_speed) {
    speed = p_speed;
}

float GridSystem::get_speed() {
    return speed;
}
