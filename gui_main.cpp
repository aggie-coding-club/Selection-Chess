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
#include "human_runner.h"
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

    PlayerRunner* whiteEngine = nullptr;
    PlayerRunner* blackEngine = nullptr;
    int numHumans = 0;
    if (argc == 3) { // TODO: this is a gross way of handling command line args, if we stick with this for long enough we should improve this
        if (std::string(argv[1]) == "human") {
            whiteEngine = new HumanRunner(std::string("Human ") + std::to_string(++numHumans));
        } else {
            whiteEngine = new EngineRunner(std::string(argv[1]));
        }
        if (!whiteEngine->init()) {
            std::cerr << "Could not add engine at '" << argv[1] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (std::string(argv[2]) == "human") {
            blackEngine = new HumanRunner(std::string("Human ") + std::to_string(++numHumans));
        } else {
            blackEngine = new EngineRunner(std::string(argv[2]));
        }
        if (!blackEngine->init()) {
            std::cerr << "Could not add engine at '" << argv[2] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        std::cerr << "Incorrect number of params. Usage:\n\t" << argv[0] << " <player> <player>\n"
        "where <player> is either the path to a chess engine, or is the string 'human' to indicate manual entry will be used for that color.\n"
        "Note that the first <player> is who plays white, and the second is who plays black." << std::endl;
        exit(EXIT_FAILURE);
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

