#include "grid_system.h"
#include "gdn_utils.h"
#include <Node2D.hpp>
#include <TileMap.hpp>

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
}

void GridSystem::set_speed(float p_speed) {
    speed = p_speed;
}

float GridSystem::get_speed() {
    return speed;
}
