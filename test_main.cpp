#include "test_macros.h"

#include "array_board.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "game.h"
#include "min_max.h"
#include "cmd_tokenizer.h" 

int main() {
    std::cout << "Conducting tests..." << std::endl;
    // Meta
    TEST("meta 1", {
        OPT_CASE("failed opt", false);
        OPT_CASE("success multiCheck", MULTI_CHECK(
            CHECK("1", true);
            CHECK("2", true);
            CHECK("3", true);
        ));
        REQ_CASE("req multiCheck", MULTI_CHECK(
            int i = 1;
            CHECK("1", true);
            i++;
            CHECK("2", true);
            CHECK("3", false);
            CHECK("4", false);
        ));
        OPT_CASE("opt F after req F", false);
        OPT_CASE("opt S after req F", true);
        REQ_CASE("req S after req F", true);
        REQ_CASE("req F after req F", false);
    });
    TEST("meta 2", {
        OPT_CASE("succeed opt", true);
        OPT_CASE("failed opt", false);
        for (int i = 0; i < 5; i++) {
            REQ_CASE("loop " + std::to_string(i), MULTI_CHECK(
                CHECK("a", true);
                CHECK("b", i != 2);
                CHECK("c", true);
            ));
        }
        OPT_CASE("succeed opt", false);
    });

    TEST("StandardArray object", {
        StandardArray sa ("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
        // std::cout << sa.dumpAsciiArray() << std::endl;
        // TODO: low priority: idk, i feel like something may be flipped?
        REQ_CASE("fenn ctor dim", sa.m_dimensions == Coords(18, 8));
        REQ_CASE("fenn ctor 0,0", sa.m_array[0] == W_ROOK);
        REQ_CASE("fenn ctor r/f ~swap", sa.m_array[7] == W_ROOK); // make sure we haven't flipped rank/file
        OPT_CASE("fenn ctor col order", sa.m_array[3] == W_QUEEN); // make sure we haven't reversed column order
        OPT_CASE("fenn ctor invld fill", sa.m_array[9] == INVALID); // check padding is there
        REQ_CASE("fenn ctor 1Darr wrap", sa.m_array[18*7] == B_ROOK); // check wrap matches dimension given
    });

    TEST("CmdTokenizer", {
        CmdTokenizer t("feature done=0 myname=\"Jeff\" debug=1 done=1");
        std::string token;
        REQ_CASE("cmd", t.next() == "feature");
        OPT_CASE("param 1", t.next() == "done");
        OPT_CASE("eqls", t.next() == "=");
        OPT_CASE("val bool", t.next() == "0");
        OPT_CASE("param 2", t.next() == "myname");
        t.next();
        OPT_CASE("val string", t.next() == "\"Jeff\"");
        t.next(); t.next(); t.next(); // debug=1
        t.next(); t.next(); t.next(); // done=1
        OPT_CASE("completed/EOF", t.next().front() == EOF);
    });

    Game game = Game("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1", "testing/test1.rules");

    // Change display settings like this:
    game.m_board->m_printSettings.m_tileFillChar = '-';
    game.m_board->m_printSettings.m_height = 1;
    game.m_board->m_printSettings.m_width = 2;
    game.m_board->m_printSettings.m_showCoords = true;

    // std::cout << game.print() << std::endl;

    TEST("Overriding initialization of game state", {
        DModCoords sampleCoords = algebraicToCoords("b6");
        REQ_CASE("sample orig", game.m_board->getPiece(sampleCoords) == B_PAWN);
        game.m_board->init("rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR w 0 1");
        OPT_CASE("sample updated", game.m_board->getPiece(sampleCoords) == EMPTY);
        // std::cout << game.m_board->toSfen() << std::endl;
        OPT_CASE("fenn out", game.m_board->toSfen() == "rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR"); // FIXME: game info got truncated, but we need to print that too
    });
    TEST("Int wrapping comparison", {
        OPT_CASE("rel0 is 0", coordLessThan(1, 5, 0) == true);
        OPT_CASE("rel0 between", coordLessThan(1, 5, 2) == false);
        OPT_CASE("rel0 is 0", coordLessThan(5, 1, 0) == false);
    });
    TEST("Int wrapping distance", {
        OPT_CASE("rel0 is 0, >", coordDistance(5, 1, 0) == 4);
        OPT_CASE("rel0 is 0, <", coordDistance(1, 5, 0) == 4);
        OPT_CASE("rel0 between, >", coordDistance(5, 1, 2) == -4);
        OPT_CASE("rel0 between, <", coordDistance(1, 5, 2) == -4);
    });

    TEST("DModCoord algebraic converions", {
        DModCoords c1 = std::make_pair(3,5);
        DModCoords c2 = std::make_pair(129,50);
        REQ_CASE("DMCToAlg 1", coordsToAlgebraic(c1) == "d5"); // single digits
        REQ_CASE("DMCToAlg 2", coordsToAlgebraic(c2) == "dz50"); // multiple digits

        REQ_CASE("AlgToDMC 1", algebraicToCoords(coordsToAlgebraic(c1)) == c1); // try converting them back
        REQ_CASE("AlgToDMC 2", algebraicToCoords(coordsToAlgebraic(c2)) == c2);
        OPT_CASE("AlgToDMC 3", algebraicToCoords("zz999") == DModCoords(701, 999)); // max value
    });

    TEST("Reading move algebraic", {
        DModCoords c1 = algebraicToCoords("d5");
        DModCoords c2 = algebraicToCoords("dz50");
        PieceMove m1(c1, c2);
        REQ_CASE("MoveToAlg", m1.algebraic() == "d5dz50");

        std::unique_ptr<Move> readMove = readAlgebraic(m1.algebraic());
        OPT_CASE("pieceMove alg", readMove->algebraic() == "d5dz50");
        OPT_CASE("pieceMove type", readMove->m_type == PIECE_MOVE);
        readMove = readAlgebraic("Sp2r2g999");
        OPT_CASE("tileMove alg", readMove->algebraic() == "Sp2r2g999");
        OPT_CASE("tileMove type", readMove->m_type == TILE_MOVE);
        readMove = readAlgebraic("Dp2r2b5");
        OPT_CASE("tileDel alg", readMove->algebraic() == "Dp2r2b5");
        OPT_CASE("tileDel type", readMove->m_type == TILE_DELETION);
        readMove = readAlgebraic("Sp2r2g999F");
        OPT_CASE("tileM F alg", readMove->algebraic() == "Sp2r2g999F");
        OPT_CASE("tileM F type", readMove->m_type == TILE_MOVE);
        readMove = readAlgebraic("Sp2r2g999R2");
        OPT_CASE("tileM R alg", readMove->algebraic() == "Sp2r2g999R2");
        OPT_CASE("tileM R type", readMove->m_type == TILE_MOVE);
        readMove = readAlgebraic("Sp2r2g999R2F");
        OPT_CASE("tileM RF alg", readMove->algebraic() == "Sp2r2g999R2F");
        OPT_CASE("tileM RF type", readMove->m_type == TILE_MOVE);
    });

    game.reset("rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
    // std::cout << game.print() << std::endl;

    TEST("apply/undo PieceMoves", {
        // TODO: the additional fenn info, e.g. "w 0 1", should not be truncated. Would be nice in the future to have a separate function for just truncated fenn, and call that here
        REQ_CASE("init fenn", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/18/PPPPPPPP/RNBQKBNR");
        game.applyMove(std::move(readAlgebraic("b0a2")));
        REQ_CASE("apply", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/N17/PPPPPPPP/R1BQKBNR");
        // std::cout << game.print() << std::endl;
        // std::cout << game.m_board->printPieces() << std::endl;

        std::shared_ptr<PieceMove> m2(new PieceMove(algebraicToCoords("a2"), algebraicToCoords("c3")));
        m2->m_capture = B_PAWN;
        game.applyMove(m2);
        REQ_CASE("apply capture", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2N5(6)2/18/PPPPPPPP/R1BQKBNR");

        game.undoMove();
        REQ_CASE("undo", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/N17/PPPPPPPP/R1BQKBNR");
        game.undoMove();
        REQ_CASE("undo capture", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/18/PPPPPPPP/RNBQKBNR");
    });

    TEST("apply/undo TileMoves", {

        // from right extrema to bottom extrema
        OPT_CASE("apply ERED", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4g997")));
            std::cout << game.print() << std::endl;
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)3/8(6)1/15/PPPPPPPP/RNBQKBNR/(6)1/(6)1/(6)3");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));

        // from right extrema to left extrema
        OPT_CASE("apply EREL", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4zx2")));
            CHECK("applied board state", game.m_board->toSfen() == "(3)rnbqkbnr/(3)pppppppp/(3)8/1(2)8(4)3/1(2)8(6)1/18/(3)PPPPPPPP/(3)RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        std::cout << "\n\n" << std::endl;

        // from left extrema to right extrema
        OPT_CASE("apply ELER", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sa0a7s0")));
            CHECK("applied board state", game.m_board->toSfen() == "nbqkbnr(10)r/ppppppp(10)p/7(10)1/7(4)4(2)1/7(6)2(2)1/18/PPPPPPP(10)P/NBQKBNR(10)R");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        std::cout << "\n\n" << std::endl;

        // from left extrema to bottom left extrema
        OPT_CASE("apply ELEBL", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sa0a7a994")));
            CHECK("applied board state", game.m_board->toSfen() == "(1)nbqkbnr/(1)ppppppp/(1)7/(1)7(4)4/(1)7(6)2/(1)17/rPPPPPPP/pNBQKBNR/1/1/1/1/P/R");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        std::cout << "\n\n" << std::endl;

        // from mid to top extrema
        OPT_CASE("apply mEU", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Se1f3c8")));
            CHECK("applied board state", game.m_board->toSfen() == "(2)2/(2)2/(2)PP/rnbqkbnr/pppppppp/8/8(4)4/4(2)2(6)2/4(2)12/PPPP(2)PP/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));

        // from right extrema to top extrema
        OPT_CASE("apply EREU", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4c8")));
            CHECK("applied board state", game.m_board->toSfen() == "(2)1/(2)1/(2)3/rnbqkbnr/pppppppp/8/8(4)3/8(6)1/15/PPPPPPPP/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));

        OPT_CASE("apply EREUL", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4zy8")));
            CHECK("applied board state", game.m_board->toSfen() == "1/1/3/(2)rnbqkbnr/(2)pppppppp/(2)8/(2)8(4)3/(2)8(6)1/(2)15/(2)PPPPPPPP/(2)RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        std::cout << "\n\n" << std::endl;

        OPT_CASE("apply EUm", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sa7h7i1")));
            CHECK("applied board state", game.m_board->toSfen() == "pppppppp/8/8(4)4/8(6)2/18/PPPPPPPPrnbqkbnr/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));

        OPT_CASE("bad apply 1", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned false", game.applyMove(readAlgebraic("Sp2r4zy2")) == false); // illegal move that would corrupt board //TODO: maybe should be left up to isLegal checker, and not tested here?
            CHECK("board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR"); // make sure board is in tact after failed apply
        ));

        // TileMove destination into the middle of nowhere, breaking continuity. // TODO: maybe this should be handled by isLegal?
        // OPT_CASE("bad apply 2", MULTI_CHECK(
        //     game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
        //     std::cout << game.print() << std::endl;
        //     CHECK("returned false", game.applyMove(readAlgebraic("Sp2r4zy10")) == false);
        //     CHECK("board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        // ));
    });

    TEST("apply/undo TileDeletions", {
        // As in 'walking the plank', these tiles depend on previous ones to connect them back to the rest of the board.
        OPT_CASE("plank 1", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Dm4n4o4")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(7)1/8(6)2/18/PPPPPPPP/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        // Test opposite read direction, in case anything funny happening with continuity checks.
        OPT_CASE("plank 2", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Do4n4m4")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(7)1/8(6)2/18/PPPPPPPP/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        // plank, but also is maximum
        OPT_CASE("max plank 1", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Dp2q2r2")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/15/PPPPPPPP/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
        // Test opposite read direction, in case anything funny happening with extrema updates.
        OPT_CASE("max plank 2", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Dr2q2p2")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/15/PPPPPPPP/RNBQKBNR");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR");
        ));
    });

    // TEST("PieceMove generation", {
    //     // TODO: implement tests, using different rulesets, etc.
    //     game.reset("P2R(2)p/1p3NB/BQp1/(3)1p w 0 1"); // simple case to play with
    //     std::cout << game.print() << std::endl;

    //     std::cout << "Possible moves: [" << std::flush;
    //     auto moves = game.m_board->getMoves(game.m_turn);
    //     for (auto &move : moves) {
    //         std::cout << move->algebraic() << ", ";
    //     }
    //     std::cout << "\b\b] \b" << std::endl;
    // });

    TEST("TileMove generation", {
        // CAUTION: overshadowing of game variable!
        Game game = Game("2 w 0 1", "testing/tileMove1.rules");
        OPT_CASE("simplest", MULTI_CHECK(
            game.reset("2 w 0 1");
            std::cout << game.print() << std::endl;

            std::cout << "Possible moves: [" << std::flush;
            auto moves = game.m_board->getMoves(game.m_turn);
            for (auto &move : moves) {
                std::cout << move->algebraic() << ", ";
            }
            std::cout << "\b\b] \b" << std::endl;

        ));
    });

    std::cout << "Done testing, for now, at\n" << WHERE << std::endl;
    return 0;

    std::cout << "Resetting to a simpler board" << std::endl;
    game.reset("P2/1K1/1pk w 0 1"); // simple case to play with
    std::cout << game.print() << std::endl;

    std::cout << "Possible moves: [" << std::flush;
    auto moves = game.m_board->getMoves(game.m_turn);
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
    // result = minmax(&game, 3, negaHistory);
    // std::cout << negaHistory;
    // std::cout << "At depth 3, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
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

    std::cout << "Done testing!" << std::endl;
    return 0;
}