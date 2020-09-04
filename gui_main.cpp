#include <cstdint>
#include <stack>
#include <iostream>
#include <climits>

#include "constants.h"
#include "utils.h"
#include "board.h"

int main() {
    std::cout << ASCII_LOGO << std::endl;
    Board guiBoard = Board("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    dout << "Done initializing board" << std::endl;

    std::cout << guiBoard.getAsciiBoard() << std::endl;
    std::cout << guiBoard.getAsciiBoard(true) << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 5, 5) << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 1, 1, '-') << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 2, 2) << std::endl;
    std::cout << guiBoard.getAsciiBoard(true, 4, 4, '.') << std::endl;
    return 0;
}

