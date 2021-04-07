#include <cstdint>
#include <stack>
#include <iostream>
#include <climits>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "board.h"
#include "engine_runner.h"
#include "tokenizer.h" // only here for the debug test

bool addEngine(std::string _enginePath, EngineRunner*& engineColor) {
    // TODO: implement
    dout << "here we would start the engine process " << _enginePath << " and do some stuff to it" << std::endl;
    engineColor = new EngineRunner();
    engineColor->init(_enginePath);
    return true;
}

int main(int argc, char *argv[]) {
    std::cout << ASCII_LOGO << std::endl;
    dout << "dout is enabled" << std::endl;
    tdout << "tdout is enabled" << std::endl;
    // initialize GUI input thread and/or godot stuff

    // for now, let's assume engines are passed via command line

    EngineRunner* whiteEngine = nullptr;
    EngineRunner* blackEngine = nullptr;
    switch (argc) { // TODO: this is a gross way of handling command line args, if we stick with this for long enough we should improve this
    case 1:
        std::cout << "Note: To have engine(s) play, specify the path of their executables as command line parameters." << std::endl;
        break;
    case 3:
        if (!addEngine(argv[2], blackEngine)) {
            std::cerr << "Could not add engine at '" << argv[2] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }
    // fall through
    case 2:
        if (!addEngine(argv[1], whiteEngine)) {
            std::cerr << "Could not add engine at '" << argv[1] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }
        break;
    default:
        std::cerr << "Incorrect number of params. Usage:\n\t" << argv[0] << " [engine 1] [engine 2]" << std::endl;
        exit(EXIT_FAILURE);
        break;
    }

    // TODO: spin up input thread / Godot interface

    // whiteEngine->init()
    // blackEngine->init()

    // Debugging prints, // TODO: remove later
    dout << "testing Tokenizer..." << std::endl;
    Tokenizer t("feature done=0 myname=\"Jeff\" debug=1 done=1");
    std::string token;
    do {
        token = t.next();
        dout << "\t[" << token << "]" << std::endl;
    } while (token.front() != EOF);
    dout << "done testing Tokenizer" << std::endl;

    Board guiBoard = Board("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    dout << "Done initializing board" << std::endl;

    std::cout << guiBoard.getAsciiBoard(true, 1, 1, '-') << std::endl;
    return 0;
}

