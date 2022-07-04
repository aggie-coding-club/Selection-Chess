#include "grid_system.h"
#include "gdn_utils.h"
#include "../chess/game.h"

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
    boardTileMap = (TileMap*) get_node("NodeBoard/BoardTileMap");
    pieceTileMap = (TileMap*) get_node("NodeBoard/PieceTileMap");
    highlightsTileMap = (TileMap*) get_node("NodeBoard/HighlightsTileMap");
    pieceTileMapFloating = (TileMap*) get_node("FloatingNodeBoard/PieceTileMap");
    boardTileMapFloating = (TileMap*) get_node("FloatingNodeBoard/BoardTileMap");

    camera = (Camera2D*) get_node("../Camera2D");

    pieceTileMap->clear();
    boardTileMap->clear();
    highlightsTileMap->clear();
    pieceTileMapFloating->clear();
    boardTileMapFloating->clear();

    game = std::make_unique<Game>("P2R(2)p/1p3NR/BQp1/(3)1p w 0 1", "rules/piecesOnly.rules");

    dlog(game->print());

    redrawBoard();
    prevChunk = getChunk();
}

void GridSystem::setCell(int _x, int _y, SquareEnum _squareVal, bool _isSelected, bool _isFloating, TileMapEnum _highlight) {
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
        pieceTileMapFloating->set_cell(_x, _y, getTMFromSquare(_squareVal));
        highlightsTileMap->set_cell(_x, _y, TM_EMPTY); // cannot highlight floating tiles

    } else {
        boardTileMapFloating->set_cell(_x, _y, TM_EMPTY);
        pieceTileMapFloating->set_cell(_x, _y, TM_EMPTY);
        boardTileMap->set_cell(_x, _y, _isSelected? TM_TILE_HIGHLIGHTED : TM_TILE);
        pieceTileMap->set_cell(_x, _y, getTMFromSquare(_squareVal));
        highlightsTileMap->set_cell(_x, _y, _highlight);
    }
}

SignedCoords GridSystem::getCoordsFromGlobalPos(Vector2 _global) {
    auto indicesRaw = boardTileMap->world_to_map(boardTileMap->to_local(_global));
    // Note that y is negated so that up is increased coords
    SignedCoords indices ((int) indicesRaw.x, (int) -indicesRaw.y);
    return indices;
}

SignedCoords GridSystem::getChunk() {
    // TileMap indices where center of camera currently is
    SignedCoords cameraIndices = getCoordsFromGlobalPos(camera->get_global_position());
    // Get chunk camera center is in. Have to convert to signed so division works as expected.
    SignedCoords cameraChunk (divFloor(cameraIndices.file, (signed) DAModulus), divFloor(cameraIndices.rank, (signed) DDModulus));
    return cameraChunk;
}

void GridSystem::redrawBoard() {
    dlog("redrawing...");
    pieceTileMap->clear();
    boardTileMap->clear();
    highlightsTileMap->clear();
    pieceTileMapFloating->clear();
    boardTileMapFloating->clear();

    auto baseCoords = game->m_board->m_displayCoordsZero;
    StandardArray sa = game->m_board->standardArray();

    SignedCoords centerChunk = getChunk();
    // dlog("centerchunk f=", centerChunk.file, " r=", centerChunk.rank);

    // Draw board in each chunk. This achieves the 'wrap-around' effect, as the center chunk is surrounded by other
    // chunks, so if we are at the edge of the center chunk we see a duplicate of its other side.
    // This can also be interpretted as there are infinite chunks in each direction, and we only load the 9 that we 
    // might see.
    for (int chunkNumOffsetR = -1; chunkNumOffsetR <= 1; chunkNumOffsetR++) {
        for (int chunkNumOffsetF = -1; chunkNumOffsetF <= 1; chunkNumOffsetF++) {
            // Draw board
            SignedCoords chunkOrigin (centerChunk.file + chunkNumOffsetF, centerChunk.rank + chunkNumOffsetR);

            for (auto f = 0; f < sa.m_dimensions.file; ++f) {
                for (auto r = 0; r < sa.m_dimensions.rank; ++r) {
                    SquareEnum square = sa.at(f, r);
                    if (square >= VOID) continue;

                    // Copy and modify coords in DMod space
                    DModCoords tileCoordsDM = baseCoords;
                    tileCoordsDM.file += f;
                    tileCoordsDM.rank -= r; // FIXME: why is rank already negated here?

                    // Convert DMod coords into SignCoords
                    SignedCoords tileCoords (chunkOrigin.file*DAModulus + tileCoordsDM.file.m_value, chunkOrigin.rank*DDModulus + tileCoordsDM.rank.m_value);
                    setCell(tileCoords.file, -tileCoords.rank, square);
                }
            }
            // setCell(chunkOrigin.file * DAModulus, -chunkOrigin.rank * DDModulus, B_ROOK);
        }
    }
    // setCell(centerChunk.file * DAModulus, -centerChunk.rank * DDModulus, W_ROOK);
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

    // Redraw board if camera has moved into a new chunk
    auto chunk = getChunk();
    if (prevChunk != chunk) {
        redrawBoard();
        prevChunk = chunk;
    }
    
    auto mouseIndices = boardTileMap->world_to_map(boardTileMap->to_local(boardTileMap->get_global_mouse_position()));
    // set_cell((int) mouseIndices.x, (int) -mouseIndices.y, W_KING);
    // dlog("floating mouse x=", mouseIndices.x, " mouse y=", mouseIndices.y);
    // dlog("mouse f=", mouseIndices.x, " mouse r=", -mouseIndices.x);
}

void GridSystem::set_speed(float p_speed) {
    speed = p_speed;
}

float GridSystem::get_speed() {
    return speed;
}
