#include <cstdint>
#include <stack>
#include <iostream>
#include <climits>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "board.h"

bool addEngine(std::string) {
    // TODO: implement
    dout << "here we would start the engine process and do some stuff to it" << std::endl;
    return true;
}

int main(int argc, char *argv[]) {
    std::cout << ASCII_LOGO << std::endl;
    // initialize GUI input thread and/or godot stuff
    // for now, let's assume engines are passed via command line
    switch (argc) { // TODO: this is a gross way of handling command line args, if we stick with this for long enough we should improve this
    case 1:
        std::cout << "Note: To have engine(s) play, specify the path of their executables as command line parameters." << std::endl;
        break;
    case 3:
        if (!addEngine(argv[2])) {
            std::cerr << "Could not add engine at '" << argv[2] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }
    // fall through
    case 2:
        if (!addEngine(argv[1])) {
            std::cerr << "Could not add engine at '" << argv[1] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }
        break;
    default:
        std::cerr << "Incorrect number of params. Usage:\n\t" << argv[0] << " [engine 1] [engine 2]" << std::endl;
        exit(EXIT_FAILURE);
        break;
    }


    // Debugging prints, // TODO: remove later
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

