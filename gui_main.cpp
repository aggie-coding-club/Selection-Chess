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
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.getTile(std::pair<short, short>(-1,-1)));

    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(-2,0)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(0,0)));
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(0,-1)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(1,0)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.getTile(std::pair<short, short>(0,0)));

    guiBoard.m_tiles.push_back(new Tile(W_KING, std::pair<short, short>(-2,1)));
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(-2,0)));
    guiBoard.m_tiles.push_back(new Tile(W_PAWN, std::pair<short, short>(-1,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.push_back(new Tile(B_KING, std::pair<short, short>(0,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(0,0)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(1,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(1,0)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(2,1)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);

    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(-2,2)));
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(-2,1)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(0,2)));
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(0,1)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(1,2)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(1, 1)));
    guiBoard.m_tiles.push_back(new Tile(EMPTY, std::pair<short, short>(2,2)));
    guiBoard.m_tiles.back()->SetAdjacent(LEFT, guiBoard.m_tiles[guiBoard.m_tiles.size() - 1]);
    guiBoard.m_tiles.back()->SetAdjacent(DOWN, guiBoard.getTile(std::pair<short, short>(2,1)));
    std::cout << guiBoard.getAsciiBoard() << std::endl;
    std::cout << guiBoard.getAsciiBoard(true) << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 5, 5) << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 1, 1, '-') << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 2, 2) << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 4, 4, '.') << std::endl;
    return 0;
}

