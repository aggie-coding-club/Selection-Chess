#include "../chess/array_board.h"
#include "../chess/chess_utils.h"
#include "../chess/constants.hpp"
#include "../chess/move.h"
#include "../chess/game.h"
#include "min_max.h"
#include "../chess/cmd_tokenizer.h"

#include <iostream>

// TODO: how should board be initialized if no setboard is given?
// TODO: how to specify rules file?
int xboardLoop() {
    Game game("2 w 0 1", "rules/test1.rules"); // TODO: not sure what default board config should be, or if there should even be one.

    std::string line;

    while (true) {
        if (std::getline(std::cin, line)) {
            std::cout << "# Hippo read [" << line << "]" << std::endl;
            CmdTokenizer tokenizer(line);
            std::string cmd = tokenizer.next();
            if (cmd == "quit") {
                std::cout << "# Hippo says goodbye." << std::endl;
                break;
            } else if (cmd == "xboard") {
                std::cout << std::endl;
            } else if (cmd == "protover") { // Assumes protocol 2
                // std::cout << "feature done=0" << std::endl;
                std::cout << "# sending features" << std::endl;
                std::cout << "feature debug=1 analyze=0 colors=0 myname=\"Hippocrene Engine 0.0.0\"" << std::endl;
                std::cout << "feature done=1" << std::endl;
            } else if (cmd == "setboard") {
                std::string sfen = tokenizer.nextSfen();
                game.reset(sfen);
                std::cout << "# Set board with sfen '" << sfen << "'" << std::endl;
                dlog("Double-checking, board has a sfen of ", game.m_board->toSfen());
            } else if (cmd == "go") {
                const int MAX_GT_DEPTH = 3; // Max depth to explore gametree. //TODO: this should be the variable of an anytime algorithm.
                dlog("running minmax at depth ", MAX_GT_DEPTH);
                auto minmaxResult = negamaxAB(&game, MAX_GT_DEPTH);
                game.applyMove(minmaxResult.second);
                std::cout << "move " << minmaxResult.second->algebraic() << std::endl;
            } else if (cmd == "print") { // not defined in xboard, but useful for debugging
                dlog(game.print());
            } else if (cmd == "accepted" || cmd == "rejected" || cmd == "new" || cmd == "variant") {
                // TODO: implement some of these commands
                // KLUDGE: instead of checking if an unknown command is actually a move, we just assume that we caught all the types of commands with this condition
                // ignore
                dlog("Unimplemented/ignored command [", cmd, "]");
            } else { // TODO: Check if the command given is actually a move, and is valid. Caution with sanity checks on isMove, being this exposed.
                dlog("recieved move [", cmd, "]");
                game.applyMove(readAlgebraic(cmd));
            }
        } else {
            std::cout << "# Hippo getline failed" << std::endl;
        }
    }
    return 0;
}

int main() {
    std::cout << "# This is the Hippocrene Engine. Please select one of the modes below by entering its [label]:" << std::endl; // TODO: because portable timeouts for io ops have not been added yet, this has to be a comment. 
    std::cout << "#\t> [xboard] For other software to interface with" << std::endl;
    // std::cout << "#\t> [test] For developers to test out features" << std::endl;
    std::string cmd;

    // start interface thread

    for (;;) {
        if (std::getline(std::cin, cmd)) {
            if (cmd == "xboard") {
                return xboardLoop();
            } else {
                std::cout << "# Unknown label [" << cmd << "]" << std::endl;
            }
        } else {
            std::cout << "# Hippo getline failed" << std::endl;
            return -1;
        }
    }
}