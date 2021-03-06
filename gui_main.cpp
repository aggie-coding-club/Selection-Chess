#include "constants.h"
#include "utils.h"
#include "game.h"
#include "board.h"
#include "human_runner.h"
#ifndef NO_BOOST
    #include "engine_runner.h"
#endif

// only here for the debug test
#include "cmd_tokenizer.h" 
#include "move.h"

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
    } else if (argc == 2 && std::string(argv[1]) == "test") {
        debugTests();
    } else {
        std::cerr << "Incorrect number of params. Usage:\n\t" << argv[0] << " <player> <player>\n"
        "where <player> is either the path to a chess engine, or is the string 'human' to indicate manual entry will be used for that color.\n"
        "Note that the first <player> is who plays white, and the second is who plays black.\n"
        "Alternative usage:\n\t" << argv[0] << " test\nTo run debugging tests." << std::endl;
        exit(EXIT_FAILURE);
    }

    // TODO: spin up input thread / Godot interface

    return 0;
}

bool debugTests() {
    dout << "running debug tests" << std::endl;

    dout << "testing StandardArray object " << std::endl;
    StandardArray sa ("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    dout << sa.dumpAsciiArray() << std::endl; // TODO: low priority: idk if this means I got something flipped somewhere, probably does.

    // DLLBoard guiBoard = DLLBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    // ArrayBoard guiBoard = ArrayBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    Game game = Game("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");

    dout << "created Game object" << std::endl;

    // Change display settings like this:
    game.m_board->m_printSettings.m_width = 2;
    game.m_board->m_printSettings.m_tileFillChar = '`';
    game.m_board->m_printSettings.m_showCoords = true;

    std::cout << game.print() << std::endl;

    dout << "testing CmdTokenizer..." << std::endl;
    CmdTokenizer t("feature done=0 myname=\"Jeff\" debug=1 done=1");
    std::string token;
    do {
        token = t.next();
        dout << "\t[" << token << "]" << std::endl;
    } while (token.front() != EOF);
    dout << "done testing CmdTokenizer" << std::endl;

    dout << "Testing overriding initialization" << std::endl;
    std::cout << game.print() << std::endl;
    game.m_board->init("rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR w 0 1");
    std::cout << game.print() << std::endl;
    dout << "min coords: " << (signed int) game.m_board->m_minCoords.first << ", " << (signed int) game.m_board->m_minCoords.second << std::endl;
    dout << "max coords: " << (signed int) game.m_board->m_maxCoords.first << ", " << (signed int) game.m_board->m_maxCoords.second << std::endl;
    std::cout << game.m_board->toSfen() << std::endl;

    dout << "Testing int wrapping..." << std::endl;
    dout << coordLessThan(1, 5, 2) << std::endl;
    dout << coordLessThan(1, 5, 0) << std::endl;
    dout << coordDistance(5, 1, 0) << std::endl;
    dout << coordDistance(1, 5, 0) << std::endl;
    dout << coordDistance(5, 1, 2) << std::endl;
    dout << coordDistance(1, 5, 2) << std::endl;
    dout << "Done testing int wrapping." << std::endl;

    dout << "Testing Algebraic Notation conversions... " << std::endl;
    DModCoords c1 = std::make_pair(3,5);
    DModCoords c2 = std::make_pair(129,50);
    dout << coordsToAlgebraic(c1) << std::endl;
    dout << coordsToAlgebraic(c2) << std::endl;
    c1 = algebraicToCoords(coordsToAlgebraic(c1));
    c2 = algebraicToCoords(coordsToAlgebraic(c2));
    c2 = algebraicToCoords("zz999");
    dout << c1.first << ", " << c1.second << std::endl;
    dout << c2.first << ", " << c2.second << std::endl;
    PieceMove m1(c1, c2);
    dout << m1.algebraic() << std::endl;
    dout << "Testing readAlgebraic" << std::endl;
    std::unique_ptr<Move> readMove = readAlgebraic(m1.algebraic());
    dout << readMove->algebraic() << " with type " << readMove->m_type << std::endl;
    readMove = readAlgebraic("Sp2r2g999");
    dout << readMove->algebraic() << " with type " << readMove->m_type << std::endl;
    readMove = readAlgebraic("Dp2r2b5");
    dout << readMove->algebraic() << " with type " << readMove->m_type << std::endl;
    readMove = readAlgebraic("Sp2r2g999F");
    dout << readMove->algebraic() << " with type " << readMove->m_type << std::endl;
    readMove = readAlgebraic("Sp2r2g999R2");
    dout << readMove->algebraic() << " with type " << readMove->m_type << std::endl;
    readMove = readAlgebraic("Sp2r2g999R2F");
    dout << readMove->algebraic() << " with type " << readMove->m_type << std::endl;

    dout << "Done with all tests" << std::endl;

    return true;
}

