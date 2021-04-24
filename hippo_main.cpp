#include "dll_board.h"
#include "array_board.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "game.h"

#include <iostream>

// TODO: async search tree functions
const size_t MAX_MESSAGE = sizeof(size_t);

int xboardLoop() {
    std::string cmd;
    // init shared variables, mutexes, etc.
    DLLBoard eBoard; // engine's memory of the board

    while (true) {
        if (std::getline(std::cin, cmd)) {
            std::cout << "# Hippo read [" << cmd << "]" << std::endl;
            if (cmd == "quit") {
                std::cout << "# Hippo says goodbye." << std::endl;
                break;
            } else if (cmd == "xboard") {
                std::cout << std::endl;
            } else if (cmd == "protover 2") { //FIXME: tokenize or something
                // std::cout << "feature done=0" << std::endl;
                std::cout << "# sending features" << std::endl;
                std::cout << "feature debug=1 analyze=0 colors=0 myname=\"Hippocrene Engine 0.0\"" << std::endl;
                std::cout << "feature done=1" << std::endl;
            }
        } else {
            std::cout << "# Hippo getline failed" << std::endl;
        }
    }
    return 0;
}
int testMode() {
    std::string cmd;
    Game game("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1"); // engine's memory of the board
    game.m_board->m_printSettings.m_tileFillChar = '-';
    game.m_board->m_printSettings.m_height = 1;
    game.m_board->m_printSettings.m_width = 1;
    game.m_board->m_printSettings.m_showCoords = true;

    std::cout << game.print() << std::endl;

    Move m1(std::make_pair(1,0), std::make_pair(0, 2));
    game.applyMove(m1);
    std::cout << game.print() << std::endl;

    Move m2(std::make_pair(0, 2), std::make_pair(2, 1));
    m2.m_capture = W_PAWN;
    game.applyMove(m2);
    std::cout << game.print() << std::endl;

    game.undoMove();
    std::cout << game.print() << std::endl;
    game.undoMove();
    std::cout << game.print() << std::endl;

    game.applyMove(m1);
    std::cout << game.print() << std::endl;
    game.applyMove(m2);
    std::cout << game.print() << std::endl;
    game.undoMove(2);
    std::cout << game.print() << std::endl;

    std::cout << "Done testing" << std::endl;
    return 0;
}

int main() {
    std::cout << "# This is the Hippocrene Engine. Please select one of the modes below by entering its [label]:" << std::endl; // TODO: because portable timeouts for io ops have not been added yet, this has to be a comment. 
    std::cout << "#\t> [xboard] For other software to interface with" << std::endl;
    std::cout << "#\t> [test] For developers to test out features" << std::endl;
    std::string cmd;

    // start interface thread

    for (;;) {
        if (std::getline(std::cin, cmd)) {
            if (cmd == "xboard") {
                return xboardLoop();
            } else if (cmd == "test") {
                return testMode();
            } else {
                std::cout << "# Unknown label [" << cmd << "]" << std::endl;
            }
        } else {
            std::cout << "# Hippo getline failed" << std::endl;
            return -1;
        }
    }
}