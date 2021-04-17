#include <cstdint>
#include <stack>
#include <iostream>
#include <climits>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "game.h"
#include "board.h"
#include "engine_runner.h"
#include "human_runner.h"
#include "tokenizer.h" // only here for the debug test

bool addEngine(std::string _enginePath, EngineRunner*& engineColor) {
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
    // DLLBoard guiBoard = DLLBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    // ArrayBoard guiBoard = ArrayBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    Game game = Game("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");

    // Change display settings like this:
    game.m_board->m_printSettings.m_width = 2;
    game.m_board->m_printSettings.m_tileFillChar = '`';
    game.m_board->m_printSettings.m_showCoords = true;


    std::cout << game.print() << std::endl;
    dout << "Successfully exited" << std::endl;

    dout << "testing Tokenizer..." << std::endl;
    Tokenizer t("feature done=0 myname=\"Jeff\" debug=1 done=1");
    std::string token;
    do {
        token = t.next();
        dout << "\t[" << token << "]" << std::endl;
    } while (token.front() != EOF);
    dout << "done testing Tokenizer" << std::endl;


    dout << "Testing overriding initialization" << std::endl;
    std::cout << game.print() << std::endl;
    game.m_board->init("rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR w 0 1");
    std::cout << game.print() << std::endl;
    // game.m_board->init("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");

    dout << "Done with all tests" << std::endl;
    return 0;
}

