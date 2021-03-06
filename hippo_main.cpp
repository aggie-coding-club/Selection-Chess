#include "array_board.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "game.h"
#include "min_max.h"

#include <iostream>

const size_t MAX_MESSAGE = sizeof(size_t);

int xboardLoop() {
    std::string cmd;
    // init shared variables, mutexes, etc.

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
    Game game("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1"); // engine's memory of the board
    game.m_board->m_printSettings.m_tileFillChar = '-';
    game.m_board->m_printSettings.m_height = 1;
    game.m_board->m_printSettings.m_width = 2;
    game.m_board->m_printSettings.m_showCoords = true;

    std::cout << game.print() << std::endl;

    std::cout << "Testing applying moves" << std::endl;
    game.applyMove(std::shared_ptr<Move>(new PieceMove(std::make_pair(1,0), std::make_pair(0, 2))));
    std::cout << game.print() << std::endl;
    std::cout << game.m_board->printPieces() << std::endl;

    std::shared_ptr<PieceMove> m2(new PieceMove(std::make_pair(0, 2), std::make_pair(2, 1)));
    m2->m_capture = W_PAWN;
    game.applyMove(m2);
    std::cout << game.print() << std::endl;
    std::cout << game.m_board->printPieces() << std::endl;

    std::cout << "Testing undoing moves" << std::endl;

    game.undoMove();
    std::cout << game.print() << std::endl;
    std::cout << game.m_board->printPieces() << std::endl;
    game.undoMove();
    std::cout << game.print() << std::endl;
    std::cout << game.m_board->printPieces() << std::endl;

    std::cout << "Testing PieceMove generation" << std::endl;

    game.reset("P2R(2)p/1p3NB/BQp1/(3)1p w 0 1"); // simple case to play with
    std::cout << game.print() << std::endl;

    std::cout << "Possible moves: [" << std::flush;
    auto moves = game.m_board->getMoves(game.m_turn);
    for (auto &move : moves) {
        std::cout << move->algebraic() << ", ";
    }
    std::cout << "\b\b] \b" << std::endl;

    return 0; // FIXME: remove once we can prune down TileMoves, currently all of them is causing an overflow. Alternatively, add rule banning selection and/or deletions moves.

    std::cout << "Resetting to a simpler board" << std::endl;

    game.reset("P2/1K1/1pk w 0 1"); // simple case to play with
    std::cout << game.print() << std::endl;

    std::cout << "Possible moves: [" << std::flush;
    moves = game.m_board->getMoves(game.m_turn);
    for (auto &move : moves) {
        std::cout << move->algebraic() << ", ";
    }
    std::cout << "\b\b] \b" << std::endl;

    std::string negaHistory = "";
    std::cout << "Testing minmax depth 1" << std::endl;
    auto result = minmax(&game, 1, negaHistory);
    std::cout << negaHistory;
    std::cout << "At depth 1, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    negaHistory = "";

    game.m_turn = BLACK;

    std::cout << "Testing minmax depth 3" << std::endl;
    result = minmax(&game, 3, negaHistory);
    std::cout << negaHistory;
    std::cout << "At depth 3, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamax(&game, 3);
    std::cout << "At depth 3, negmax found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamaxAB(&game, 3);
    std::cout << "At depth 3, negmaxAB found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    negaHistory = "";

    game.m_turn = WHITE;

    std::cout << "Testing minmax depth 3" << std::endl;
    result = minmax(&game, 3, negaHistory);
    std::cout << negaHistory;
    std::cout << "At depth 3, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamax(&game, 3);
    std::cout << "At depth 3, negmax found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamaxAB(&game, 3);
    std::cout << "At depth 3, negmaxAB found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    negaHistory = "";

    std::cout << "Testing minmax depth 8" << std::endl;
    result = minmax(&game, 8, negaHistory);
    // std::cout << negaHistory;
    std::cout << "At depth 8, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamax(&game, 8);
    std::cout << "At depth 8, negmax found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamaxAB(&game, 8);
    std::cout << "At depth 8, negmaxAB found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    negaHistory = "";

    std::cout << "Testing TileMoves" << std::endl;
    game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
    std::cout << game.print() << std::endl;

    if (!game.applyMove(readAlgebraic("Sp2r4g997"))) {
        std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    }
    std::cout << game.print() << std::endl;
    // std::cout << dynamic_cast<ArrayBoard*>(game.m_board)->dumpAsciiArray() << std::endl;
    game.undoMove();
    std::cout << game.print() << std::endl;
    // std::cout << dynamic_cast<ArrayBoard*>(game.m_board)->dumpAsciiArray() << std::endl;

    // ----------- Additional tileMove test cases ----------- //
    // if (!game.applyMove(readAlgebraic("Sp2r4zx2")))
    //     std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    // std::cout << game.print() << std::endl;
    // game.undoMove();
    // std::cout << game.print() << std::endl;

    // if (!game.applyMove(readAlgebraic("Sp2r4zy8")))
    //     std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    // std::cout << game.print() << std::endl;
    // game.undoMove();
    // std::cout << game.print() << std::endl;

    // if (!game.applyMove(readAlgebraic("Sp2r4p0")))
    //     std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    // std::cout << game.print() << std::endl;
    // game.undoMove();
    // std::cout << game.print() << std::endl;

    // if (!game.applyMove(readAlgebraic("Sa0b7s0")))
    //     std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    // std::cout << game.print() << std::endl;
    // game.undoMove();
    // std::cout << game.print() << std::endl;

    // if (!game.applyMove(readAlgebraic("Sd6g6k3")))
    //     std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    // std::cout << game.print() << std::endl;
    // game.undoMove();
    // std::cout << game.print() << std::endl;
    // ------------------------ //

    std::cout << "Testing TileDeletions" << std::endl;

    if (!game.applyMove(readAlgebraic("Dp2q2r2"))) {
        std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    }
    std::cout << game.print() << std::endl;
    // std::cout << dynamic_cast<ArrayBoard*>(game.m_board)->dumpAsciiArray() << std::endl;
    game.undoMove();
    std::cout << game.print() << std::endl;
    // std::cout << dynamic_cast<ArrayBoard*>(game.m_board)->dumpAsciiArray() << std::endl;

    if (!game.applyMove(readAlgebraic("Da0a1a2a3a4a5a6a7a3p3p4")))
        std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    std::cout << game.print() << std::endl;
    game.undoMove();
    std::cout << game.print() << std::endl;

    if (!game.applyMove(readAlgebraic("Df0h0a0g0b0c0d0e0f7h7a7g7b7c7d7e7")))
        std::cout << "ERROR! Could not apply move " << WHERE << std::endl;
    std::cout << game.print() << std::endl;
    game.undoMove();
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