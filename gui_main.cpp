#include "constants.h"
#include "chess_utils.h"
#include "game.h"
#include "board.h"
#include "human_runner.h"
#ifndef NO_BOOST
    #include "engine_runner.h"
#endif

#include <cstdint>
#include <stack>
#include <iostream>
#include <climits>
#include <stdlib.h>
#include <stdio.h>

// Forward declarations
bool debugTests();

int main(int argc, char *argv[]) {
    std::cout << ASCII_LOGO << std::endl;
    // initialize GUI input thread and/or godot stuff

    // for now, let's assume engines are passed via command line

    PlayerRunner* whiteEngine = nullptr;
    PlayerRunner* blackEngine = nullptr;
    int numHumans = 0;
    if (argc == 3) { // TODO: this is a gross way of handling command line args, if we stick with this for long enough we should improve this
        if (std::string(argv[1]) == "human") {
            whiteEngine = new HumanRunner(std::string("Human ") + std::to_string(++numHumans));
        } else {
            #ifndef NO_BOOST
                whiteEngine = new EngineRunner(std::string(argv[1]));
            #else
                std::cerr << "Cannot add engine without Boost. Please recompile without the 'noboost' flag to use this feature.";
            #endif
        }
        if (!whiteEngine->init()) {
            std::cerr << "Could not add engine at '" << argv[1] << "'!" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (std::string(argv[2]) == "human") {
            blackEngine = new HumanRunner(std::string("Human ") + std::to_string(++numHumans));
        } else {
            #ifndef NO_BOOST
                blackEngine = new EngineRunner(std::string(argv[2]));
            #else
                std::cerr << "Cannot add engine without Boost. Please recompile without the 'noboost' flag to use this feature.";
            #endif
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

    std::string sfen = "rkr3/6/6/6/6/3RKR w 0 1";
    Game game(sfen, "default.rules");
    whiteEngine->setBoard(sfen);
    blackEngine->setBoard(sfen);
    std::cout << "boards init'd, going into game loop" << std::endl;
    for(;;) { // TODO: remove redundant code for both black and white
        std::shared_ptr<Move> whiteMove = std::move(whiteEngine->getMove());
        //TODO: check if legal
        game.applyMove(whiteMove);
        blackEngine->setMove(whiteMove);
        // TODO: handle if black rejects move as being legal

        std::shared_ptr<Move> blackMove = std::move(blackEngine->getMove());
        //TODO: check if legal
        game.applyMove(blackMove);
        whiteEngine->setMove(blackMove);
        // TODO: handle if white rejects move as being legal
    }
    std::cout << "Exiting SelChess GUI" << std::endl;
    return 0;
}
