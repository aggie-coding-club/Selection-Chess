#include <cstdint>
#include <stack>
#include <iostream>
#include <climits>

#include "constants.h"
#include "board.h"

int main() {
    std::cout << ASCII_LOGO << std::endl;
    Board guiBoard = Board();
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(-1,-1)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(0,-1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);

    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(0,0)));
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.m_tiles[1]);
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(1,0)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);

    guiBoard.m_tiles.push_back(new Tile(W_KING, std::pair<short, short>(-1,1)));
    guiBoard.m_tiles.push_back(new Tile(B_KING, std::pair<short, short>(0,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.m_tiles[2]);
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(1,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.m_tiles[3]);
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(2,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);

    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(0,2)));
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.m_tiles[5]);
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(1,2)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.m_tiles[6]);
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(2,2)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.m_tiles[7]);
    std::cout << guiBoard.getAsciiBoard() << std::endl;
    return 0;
}

