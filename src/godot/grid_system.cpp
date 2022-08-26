#include "grid_system.h"
#include "gdn_utils.h"
#include "../chess/game.h"

#include <Viewport.hpp>

using namespace godot;

void GridSystem::_register_methods() {
    register_method("_process", &GridSystem::_process);
    register_method("_ready", &GridSystem::_ready);
    register_method("reset_sfen", &GridSystem::reset_sfen);
    register_method("add_engine", &GridSystem::add_engine);
    // register_property<GridSystem, float>("amplitude", &GridSystem::amplitude, 10.0);
    // register_property<GridSystem, float>("speed", &GridSystem::set_speed, &GridSystem::get_speed, 1.0);

    register_signal<GridSystem>((char *)"engine_log", "player_num", GODOT_VARIANT_TYPE_INT, "text", GODOT_VARIANT_TYPE_STRING);
    register_signal<GridSystem>((char *)"sfen_update", "sfen", GODOT_VARIANT_TYPE_STRING);
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
    // TODO: figure out which of these are being used
    boardTileMap = (TileMap*) get_node("NodeBoard/BoardTileMap");
    pieceTileMap = (TileMap*) get_node("NodeBoard/PieceTileMap");
    highlightsTileMap = (TileMap*) get_node("NodeBoard/HighlightsTileMap");
    pieceTileMapFloating = (TileMap*) get_node("FloatingNodeBoard/PieceTileMap");
    boardTileMapFloating = (TileMap*) get_node("FloatingNodeBoard/BoardTileMap");

    gameSpaceNode = (Node2D*) get_owner();
    // dlog("got owner as: ", gameSpaceNode->get_name());

    camera = (Camera2D*) get_node("../Camera2D");


    m_game = std::make_unique<Game>("P2R(2)p/1p3NR/BQp1/(3)1p a0 w 0 1", "rules/piecesOnly.rules");
    emit_signal("sfen_update", m_game->toSfen().c_str());
}

void GridSystem::_process(float delta) {
    // time_passed += speed * delta;

    // Vector2 new_position = Vector2(
    //     amplitude + (amplitude * sin(time_passed * 2.0)),
    //     amplitude + (amplitude * cos(time_passed * 1.5))
    // );

    // set_position(new_position);

    // time_emit += delta;
    // if (time_emit > 1.0) {
    //     emit_signal("position_changed", this, new_position);

    //     time_emit = 0.0;
    // }

    auto mouseIndices = boardTileMap->world_to_map(boardTileMap->to_local(boardTileMap->get_global_mouse_position()));
    // set_cell((int) mouseIndices.x, (int) -mouseIndices.y, W_KING);
    // dlog("floating mouse x=", mouseIndices.x, " mouse y=", mouseIndices.y);
    // dlog("mouse f=", mouseIndices.x, " mouse r=", -mouseIndices.x);
}

void GridSystem::_input(Variant event) {
    
}
void GridSystem::reset_sfen(Variant _sfen) {
    std::string sfen = variantToStdString(_sfen);
    m_game->reset(sfen);
    // TODO: handle invalid sfen
    emit_signal("sfen_update", m_game->toSfen().c_str());
}
void GridSystem::add_engine(Variant _path, PieceColor _player) {
    std::string path = variantToStdString(_path);
    std::unique_ptr<EngineRunner> engine = std::make_unique<EngineRunner>(path);

    if (_player == WHITE) {
        m_whiteEngine = std::move(engine);
        dlog("added white engine [", path, "]");
    } else {
        m_blackEngine = std::move(engine);
        dlog("added black engine [", path, "]");
    }
}