#include "test_macros.h"

#include "../chess/array_board.h"
#include "../chess/chess_utils.h"
#include "../chess/constants.hpp"
#include "../chess/move.h"
#include "../chess/game.h"
#include "../engine/min_max.h"
#include "../chess/cmd_tokenizer.h" 
#include "../utils/coords.hpp"

#include <sstream>

int main() {
    std::cout << "Conducting tests..." << std::endl;

    dlog("Testing dlogs...");
    dlogStart("start "); dlogEnd("end");
    dlogStart("start "); dlogMid("mid "); dlogEnd("end");
    dlogStart("start "); dlogMid("mid "); dlogMid("mid "); dlogEnd("end");
    dlog("dlog\nwith\nmultilines!");

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

    TestMacros::numTestsFailed = 0; // Reset this so that meta tests don't affect the actual tests.
    TestMacros::numTestsPassed = 0;

    TEST("StandardArray object", {
        StandardArray sa ("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
        // std::cout << sa.dumpAsciiArray() << std::endl;
        // TODO: low priority: idk, i feel like something may be flipped?
        REQ_CASE("sfen ctor dim", sa.m_dimensions == UnsignedCoords(18, 8));
        REQ_CASE("sfen ctor 0,0", sa.m_array[0] == W_ROOK);
        REQ_CASE("sfen ctor r/f ~swap", sa.m_array[7] == W_ROOK); // make sure we haven't flipped rank/file
        OPT_CASE("sfen ctor col order", sa.m_array[3] == W_QUEEN); // make sure we haven't reversed column order
        OPT_CASE("sfen ctor invld fill", sa.m_array[9] == VOID); // check padding is there
        REQ_CASE("sfen ctor 1Darr wrap", sa.m_array[18*7] == B_ROOK); // check wrap matches dimension given
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

    Game game = Game("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR ca100 w 0 1", "rules/test1.rules");

    // Change display settings like this:
    game.m_board->m_printSettings.m_tileFillChar = '-';
    game.m_board->m_printSettings.m_height = 1;
    game.m_board->m_printSettings.m_width = 2;
    game.m_board->m_printSettings.m_showCoords = true;

    // std::cout << game.print() << std::endl;

    TEST("Overriding initialization of game state", {
        DModCoords sampleCoords = algebraicToCoords("cb106");
        REQ_CASE("orig sample", game.m_board->getPiece(sampleCoords) == B_PAWN);
        OPT_CASE("init full sfen", MULTI_CHECK(
            game.reset("rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR ca100 b 10 4");
            std::cout << game.m_board->toSfen() << std::endl;
            CHECK("sampled piece", game.m_board->getPiece(sampleCoords) == EMPTY);
            dlog(game.toSfen());
            CHECK("sfen position out", game.m_board->toSfen() == "rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR ca100");
            dlog(game.toSfen());
            CHECK("sfen full out", game.toSfen() == "rnbqkbnr/p6p/8/2(4)2/(2)4/18/P6P/RNBQKBNR ca100 b 10 4");
        ));
        OPT_CASE("init part sfen", MULTI_CHECK(
            game.reset("rkb/3/1(1)1/3/BKR");
            // std::cout << game.m_board->toSfen() << std::endl;
            sampleCoords = algebraicToCoords("b0");
            dlog(game.toSfen());
            CHECK("sampled piece", game.m_board->getPiece(sampleCoords) == W_KING);
            CHECK("sfen position out", game.m_board->toSfen() == "rkb/3/1(1)1/3/BKR a0");
            CHECK("sfen full out", game.toSfen() == "rkb/3/1(1)1/3/BKR a0 w 0 1");
        ));
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
    TEST("ModInt Heuristic comparators", {
        DDModInt five(5);
        DDModInt seven(7);
        DDModInt minusFive(-5);
        DDModInt minusSeven(-7);
        OPT_CASE("5 < 7", five.heurLessThan(seven) == true); // Test small
        OPT_CASE("7 !< 5", seven.heurLessThan(five) == false);
        OPT_CASE("-7 < -5", minusSeven.heurLessThan(minusFive) == true); // Test large
        OPT_CASE("-5 !< -7", minusFive.heurLessThan(minusSeven) == false);
        OPT_CASE("-5 < 5", minusFive.heurLessThan(five) == true); // Test across zero
        OPT_CASE("5 !< -5", five.heurLessThan(minusFive) == false);
        OPT_CASE("5 !< 5", five.heurLessThan(five) == false); // Test irreflexive

        OPT_CASE("5 <= 7", five.heurLessThanOrEqual(seven) == true); // Test small
        OPT_CASE("7 !<= 5", seven.heurLessThanOrEqual(five) == false);
        OPT_CASE("-7 <= -5", minusSeven.heurLessThanOrEqual(minusFive) == true); // Test large
        OPT_CASE("-5 !<= -7", minusFive.heurLessThanOrEqual(minusSeven) == false);
        OPT_CASE("-5 <= 5", minusFive.heurLessThanOrEqual(five) == true); // Test across zero
        OPT_CASE("5 !<= -5", five.heurLessThanOrEqual(minusFive) == false);
        OPT_CASE("5 <= 5", five.heurLessThanOrEqual(five) == true); // Test reflexive

        OPT_CASE("5 to 7", five.getDistTo(seven) == 2); // Test small
        OPT_CASE("7 to 5", seven.getDistTo(five) == -2);
        OPT_CASE("-7 to -5", minusSeven.getDistTo(minusFive) == 2); // Test large
        OPT_CASE("-5 to -7", minusFive.getDistTo(minusSeven) == -2);
        OPT_CASE("-5 to 5", minusFive.getDistTo(five) == 10); // Test across zero
        OPT_CASE("5 to -5", five.getDistTo(minusFive) == -10);
        OPT_CASE("5 to 5", five.getDistTo(five) == 0); // Test 0 dist
    });

    TEST("DModCoord algebraic converions", {
        DModCoords c1 = DModCoords(3,5);
        DModCoords c2 = DModCoords(129,50);
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

    game.reset("rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
    // std::cout << game.print() << std::endl;

    TEST("apply/undo PieceMoves", {
        // TODO: the additional fen info, e.g. "w 0 1", should not be truncated. Would be nice in the future to have a separate function for just truncated sfen, and call that here
        REQ_CASE("init sfen", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/18/PPPPPPPP/RNBQKBNR a0");
        REQ_CASE("applyMove()->true", game.applyMove(readAlgebraic("b0a2")));
        REQ_CASE("apply", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/N17/PPPPPPPP/R1BQKBNR a0");
        // std::cout << game.print() << std::endl;
        // std::cout << game.m_board->printPieces() << std::endl;

        std::unique_ptr<PieceMove> m2(new PieceMove(algebraicToCoords("a2"), algebraicToCoords("c3")));
        m2->m_capture = B_PAWN;
        game.applyMove(std::move(m2));
        REQ_CASE("apply capture", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2N5(6)2/18/PPPPPPPP/R1BQKBNR a0");

        game.undoMove();
        REQ_CASE("undo", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/N17/PPPPPPPP/R1BQKBNR a0");
        game.undoMove();
        REQ_CASE("undo capture", game.m_board->toSfen() == "rnbqkbnr/pp1ppppp/8/8(4)4/2p5(6)2/18/PPPPPPPP/RNBQKBNR a0");
    });

    TEST("apply/undo TileMoves", {

        // from right extrema to bottom extrema
        OPT_CASE("apply ERED", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4g997")));
            std::cout << game.print() << std::endl;
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)3/8(6)1/15/PPPPPPPP/RNBQKBNR/(6)1/(6)1/(6)3 a997");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));

        // from right extrema to left extrema
        OPT_CASE("apply EREL", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4zx2")));
            CHECK("applied board state", game.m_board->toSfen() == "(3)rnbqkbnr/(3)pppppppp/(3)8/1(2)8(4)3/1(2)8(6)1/18/(3)PPPPPPPP/(3)RNBQKBNR zx0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        std::cout << "\n\n" << std::endl;

        // from left extrema to right extrema
        OPT_CASE("apply ELER", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sa0a7s0")));
            CHECK("applied board state", game.m_board->toSfen() == "nbqkbnr(10)r/ppppppp(10)p/7(10)1/7(4)4(2)1/7(6)2(2)1/18/PPPPPPP(10)P/NBQKBNR(10)R b0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        std::cout << "\n\n" << std::endl;

        // from left extrema to bottom left extrema
        OPT_CASE("apply ELEBL", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sa0a7a994")));
            CHECK("applied board state", game.m_board->toSfen() == "(1)nbqkbnr/(1)ppppppp/(1)7/(1)7(4)4/(1)7(6)2/(1)17/rPPPPPPP/pNBQKBNR/1/1/1/1/P/R a994");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        std::cout << "\n\n" << std::endl;

        // from mid to top extrema
        OPT_CASE("apply mEU", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Se1f3c8")));
            CHECK("applied board state", game.m_board->toSfen() == "(2)2/(2)2/(2)PP/rnbqkbnr/pppppppp/8/8(4)4/4(2)2(6)2/4(2)12/PPPP(2)PP/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));

        // from right extrema to top extrema
        OPT_CASE("apply EREU", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4c8")));
            CHECK("applied board state", game.m_board->toSfen() == "(2)1/(2)1/(2)3/rnbqkbnr/pppppppp/8/8(4)3/8(6)1/15/PPPPPPPP/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));

        OPT_CASE("apply EREUL", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sp2r4zy8")));
            CHECK("applied board state", game.m_board->toSfen() == "1/1/3/(2)rnbqkbnr/(2)pppppppp/(2)8/(2)8(4)3/(2)8(6)1/(2)15/(2)PPPPPPPP/(2)RNBQKBNR zy0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        std::cout << "\n\n" << std::endl;

        OPT_CASE("apply EUm", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Sa7h7i1")));
            CHECK("applied board state", game.m_board->toSfen() == "pppppppp/8/8(4)4/8(6)2/18/PPPPPPPPrnbqkbnr/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));

        OPT_CASE("bad apply 1", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned false", game.applyMove(readAlgebraic("Sp2r4zy2")) == false); // illegal move that would corrupt board //TODO: maybe should be left up to isLegal checker, and not tested here?
            CHECK("board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0"); // make sure board is in tact after failed apply
        ));
        // The following two were corner cases generated by getMoves() that were problematic.
        OPT_CASE("bad apply 2", MULTI_CHECK(
            game.reset("P1n5 a0 w 0 1");
            CHECK("returned false", game.applyMove(readAlgebraic("Sa0a0zz999")) == false);
            CHECK("board state", game.m_board->toSfen() == "P1n5 a0");
        ));
        OPT_CASE("bad apply 3", MULTI_CHECK(
            game.reset("P1n a0 w 0 1");
            CHECK("returned false", game.applyMove(readAlgebraic("Sa0a0zz999")) == false);
            CHECK("board state", game.m_board->toSfen() == "P1n a0");
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
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Dm4n4o4")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(7)1/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        // Test opposite read direction, in case anything funny happening with continuity checks.
        OPT_CASE("plank 2", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Do4n4m4")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(7)1/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        // plank, but also is maximum
        OPT_CASE("max plank 1", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Dp2q2r2")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/15/PPPPPPPP/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
        // Test opposite read direction, in case anything funny happening with extrema updates.
        OPT_CASE("max plank 2", MULTI_CHECK(
            game.reset("rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0 w 0 1");
            CHECK("returned true", game.applyMove(readAlgebraic("Dr2q2p2")));
            CHECK("applied board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/15/PPPPPPPP/RNBQKBNR a0");
            game.undoMove();
            CHECK("undone board state", game.m_board->toSfen() == "rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR a0");
        ));
    });

    TEST("Continuity Checker", {
        // CAUTION: overshadowing of game variable!
        // Game game = Game("2 w 0 1", "rules/tileMove1.rules");
        Ruleset rules ("rules/tileMove1.rules");
        OPT_CASE("2 cont", MULTI_CHECK(
            ArrayBoard ab(rules, "2 a0 w 0 1");
            std::cout << ab.getAsciiBoard();
            CHECK("isContinuous", ab.isContiguous());
        ));
        OPT_CASE("2 non cont", MULTI_CHECK(
            ArrayBoard ab(rules, "1/(1)1 a0 w 0 1");
            std::cout << ab.getAsciiBoard();
            CHECK("isContinuous", ab.isContiguous()==false);
        ));
        OPT_CASE("cont w pieces", MULTI_CHECK(
            ArrayBoard ab(rules, "(2)3/(2)1(1)1/(2)3/1k1(1)1/pQ1(1)1/1(3)1/(1)K3 a0 w 0 1");
            std::cout << ab.getAsciiBoard();
            CHECK("isContinuous", ab.isContiguous());
        ));
        OPT_CASE("noncont w pieces", MULTI_CHECK(
            ArrayBoard ab(rules, "(2)3/(2)1(1)1/(3)2/1k1(1)1/pQ1(1)1/1(3)1/(1)K3 a0 w 0 1");
            std::cout << ab.getAsciiBoard();
            CHECK("isContinuous", ab.isContiguous()==false);
        ));
    });

    TEST("apply/undo handles nonContiguous moves", {
        OPT_CASE("two tiles", MULTI_CHECK(
            game.reset("1P a0 w 0 1");
            std::cout << game.m_board->getAsciiBoard() << std::endl;
            CHECK("returned false", game.applyMove(readAlgebraic("Sa0a0a1")) == false);
            std::cout << game.m_board->getAsciiBoard() << std::endl;
            CHECK("board state after", game.m_board->toSfen() == "1P a0");
        ));
        OPT_CASE("T", MULTI_CHECK(
            game.reset("nkP/(1)Q a0 w 0 1");
            std::cout << game.m_board->getAsciiBoard() << std::endl;
            CHECK("returned false", game.applyMove(readAlgebraic("Sb1b1b2")) == false);
            std::cout << game.m_board->getAsciiBoard() << std::endl;
            CHECK("board state after", game.m_board->toSfen() == "nkP/(1)Q a0");
        ));
        //TODO: when multiple tile moves are supported, add test cases for them here too
    });


    TEST("TileMove generation", { // TODO: remove or improve
        // CAUTION: overshadowing of game variable!
        Game game = Game("1P a0 w 0 1", "rules/tileMove1.rules");
        OPT_CASE("simplest", MULTI_CHECK(
            game.reset("1P a0 w 0 1");
            std::cout << game.print() << std::endl;

            std::cout << "Maybe moves: [" << std::flush;
            auto moves = game.m_board->getMaybeMoves(game.m_turn);
            for (auto &move : moves) {
                std::cout << move->algebraic() << ", ";
            }
            std::cout << "\b\b] \b" << std::endl;
            std::cout << "Legal moves: [" << std::flush;
            moves = game.m_board->getMoves(game.m_turn);
            for (auto &move : moves) {
                std::cout << move->algebraic() << ", ";
            }
            std::cout << "\b\b] \b" << std::endl;

        ));
    });

    std::cout << "Resetting to a simpler board" << std::endl;
    // game.reset("P2/1K1/1pk a0 w 0 1"); // simple case to play with
    game.reset("P1n a0 w 0 1"); // simpler case to play with
    std::cout << game.print() << std::endl;

    std::cout << "Possible moves: [" << std::flush;
    auto moves = game.m_board->getMaybeMoves(game.m_turn);
    for (auto &move : moves) {
        std::cout << move->algebraic() << ", ";
    }
    std::cout << "\b\b] \b" << std::endl;
    std::cout << "Possible moves: [" << std::flush;
    moves = game.m_board->getMoves(game.m_turn);
    for (auto &move : moves) {
        std::cout << move->algebraic() << ", ";
    }
    std::cout << "\b\b] \b" << std::endl;

    std::cout << game.print() << std::endl;
    std::string negaHistory = "";
    std::cout << "Testing minmax depth 1" << std::endl;
    auto result = minmax(&game, 1, negaHistory);
    // std::cout << negaHistory;
    std::cout << "At depth 1, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    negaHistory = "";

    std::cout << game.print() << std::endl;

    game.m_turn = BLACK;

    std::cout << "Testing minmax depth 3" << std::endl;
    result = minmax(&game, 3, negaHistory);
    // std::cout << negaHistory;
    // std::cout << "At depth 3, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    // result = negamax(&game, 3);
    // std::cout << "At depth 3, negmax found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    // result = negamaxAB(&game, 3);
    // std::cout << "At depth 3, negmaxAB found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    // negaHistory = "";
    std::cout << game.print() << std::endl;

    game.m_turn = WHITE;

    std::cout << "Testing minmax depth 3" << std::endl;
    result = minmax(&game, 3, negaHistory);
    // std::cout << negaHistory;
    std::cout << "At depth 3, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamax(&game, 3);
    std::cout << "At depth 3, negmax found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    result = negamaxAB(&game, 3);
    std::cout << "At depth 3, negmaxAB found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    negaHistory = "";

    // // Cut and paste this to return early
    // std::cout << game.print() << std::endl;
    // std::cout << "Done testing at " << WHERE << std::endl;
    // return 0;

    // std::cout << "Testing minmax depth 8" << std::endl;
    // result = minmax(&game, 8, negaHistory);
    // // std::cout << negaHistory;
    // std::cout << "At depth 8, score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    // result = negamax(&game, 8);
    // std::cout << "At depth 8, negmax found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    // result = negamaxAB(&game, 8);
    // std::cout << "At depth 8, negmaxAB found score is " << result.first << " and best move is " << result.second->algebraic() << std::endl;
    // negaHistory = "";


    TEST("PieceMove generation", {
        // TODO: implement tests, using different rulesets, etc.
        Game game ("P2R(2)p/1p3NR/BQp1/(3)1p a0 w 0 1", "rules/piecesOnly.rules"); // simple case to play with
        // game.reset("k1K2/n(2)p1/1(1)1(1)1/RQ1B1");
        dlog(game.print());
        dlogStart("Possible moves: [");
        auto moves = game.m_board->getMaybeMoves(game.m_turn); // annoying: can't access getPieceMoves cuz inheritence
        for (auto &move : moves) {
            dlogMid(move->algebraic(),", ");
        }
        dlogEnd("\b\b] \b");
        REQ_CASE("board 1", MULTI_CHECK(
            CHECK("correct number of moves", moves.size() == 16); // ? why was this set to 15 before? was I missing a move?
            // Expected moves:

        ));
    });

    TEST("IsLegal(PieceMove)", {
        // TODO: implement tests, using different rulesets, etc.
        Game game ("P2R(2)p/1p3NR/BQp1/(3)1p a0 w 0 1", "rules/piecesOnly.rules");
        dlog(game.print());
        // Assuming move generation is working properly, this will test all legal moves.
        OPT_CASE("vs generated", MULTI_CHECK(
            auto moves = game.m_board->getMoves(game.m_turn);
            for (auto &move : moves) {
                std::string moveName = move->algebraic();
                CHECK(moveName, game.m_board->isLegal(*move, game.m_turn));
            }
        ));
        
        OPT_CASE("slide to void b1", game.m_board->isLegal(*readAlgebraic("g2f3"), game.m_turn) == false);
        OPT_CASE("slide to void b2", game.m_board->isLegal(*readAlgebraic("g2e4"), game.m_turn) == false);
        OPT_CASE("slide to void b3", game.m_board->isLegal(*readAlgebraic("g2d5"), game.m_turn) == false);
        OPT_CASE("slide to void b4", game.m_board->isLegal(*readAlgebraic("g2c6"), game.m_turn) == false);
        OPT_CASE("slide to void b5", game.m_board->isLegal(*readAlgebraic("g2b7"), game.m_turn) == false);
        OPT_CASE("slide to void b6", game.m_board->isLegal(*readAlgebraic("g2a8"), game.m_turn) == false);
        OPT_CASE("slide to void b7", game.m_board->isLegal(*readAlgebraic("g2zz9"), game.m_turn) == false);
        OPT_CASE("slide to void b8", game.m_board->isLegal(*readAlgebraic("g2zy10"), game.m_turn) == false);

        OPT_CASE("slide to void p1", game.m_board->isLegal(*readAlgebraic("a3zz3"), game.m_turn) == false);
        OPT_CASE("slide to void p2", game.m_board->isLegal(*readAlgebraic("a3a4"), game.m_turn) == false);
        OPT_CASE("slide to void p3", game.m_board->isLegal(*readAlgebraic("a3zy3"), game.m_turn) == false);
        OPT_CASE("slide to void p4", game.m_board->isLegal(*readAlgebraic("a3g3"), game.m_turn) == false);
        OPT_CASE("slide to void p5", game.m_board->isLegal(*readAlgebraic("a3a0"), game.m_turn) == false);

        OPT_CASE("jump to void 1", game.m_board->isLegal(*readAlgebraic("f2g0"), game.m_turn) == false);
        OPT_CASE("jump to void 2", game.m_board->isLegal(*readAlgebraic("f2h3"), game.m_turn) == false);
        OPT_CASE("jump to void 3", game.m_board->isLegal(*readAlgebraic("f2g4"), game.m_turn) == false);

        OPT_CASE("capture own 1", game.m_board->isLegal(*readAlgebraic("d3a3"), game.m_turn) == false);
        OPT_CASE("capture own 2", game.m_board->isLegal(*readAlgebraic("b2c1"), game.m_turn) == false);
        OPT_CASE("capture own 3", game.m_board->isLegal(*readAlgebraic("f2d3"), game.m_turn) == false);

        OPT_CASE("misshaped jump", game.m_board->isLegal(*readAlgebraic("f2d0"), game.m_turn) == false);
        OPT_CASE("slide thru enemy", game.m_board->isLegal(*readAlgebraic("a1c3"), game.m_turn) == false);
        OPT_CASE("slide thru own", game.m_board->isLegal(*readAlgebraic("g2e2"), game.m_turn) == false);

        OPT_CASE("pawn atk vert", game.m_board->isLegal(*readAlgebraic("b2b1"), game.m_turn) == false);
        OPT_CASE("pawn atk hori", game.m_board->isLegal(*readAlgebraic("c1b1"), game.m_turn) == false);
        OPT_CASE("pawn move diag", game.m_board->isLegal(*readAlgebraic("c1d2"), game.m_turn) == false);

        OPT_CASE("empty to piece", game.m_board->isLegal(*readAlgebraic("b3a3"), game.m_turn) == false);
        OPT_CASE("empty to void 1", game.m_board->isLegal(*readAlgebraic("e2e3"), game.m_turn) == false);
        OPT_CASE("void to piece 1", game.m_board->isLegal(*readAlgebraic("a0a1"), game.m_turn) == false);
        OPT_CASE("void to empty 1", game.m_board->isLegal(*readAlgebraic("c0d0"), game.m_turn) == false);
        OPT_CASE("empty to void 2", game.m_board->isLegal(*readAlgebraic("e2zz999"), game.m_turn) == false);
        OPT_CASE("void to piece 2", game.m_board->isLegal(*readAlgebraic("zz999a1"), game.m_turn) == false);
        OPT_CASE("void to empty 2", game.m_board->isLegal(*readAlgebraic("zz999d0"), game.m_turn) == false);

        // For other types of moves, here are some cases I want to check when we do those:
        // OPT_CASE("empty deletion", game.m_board->isLegal(readAlgebraic("D"), game.m_turn) == false);

    });


    // KLUDGE: typedef these outside of macro to avoid problems with commas
    typedef Coords<int, unsigned int> HeteroInt;
    // typedef HomoCoords<int> HomoInt;
    TEST("Coords/HomoCoords class", {
        HeteroInt hetero1 (-3, 1);
        std::stringstream stream("");
        stream << hetero1;
        OPT_CASE("hetero op<<", stream.str() == "(-3, 1)");
        // HomoInt homo1 (3, -1);
        // stream = std::stringstream("");
        // stream << homo1;
        // OPT_CASE("homo op<<", stream.str() == "(3, -1)");
        HeteroInt hetero2 (5, -3);
        OPT_CASE("het-het", hetero1-hetero2 == HeteroInt(-8, 4));
        HeteroInt homo2 (5, -3);
        // OPT_CASE("homo-homo", homo1-homo2 == HomoInt(-2, 2));
        // OPT_CASE("hetero-homo", hetero1-homo1 == HeteroInt(-6, 2));
        // OPT_CASE("homo-hetero", homo1-hetero1 == HomoInt(6, -2));

        // OPT_CASE("homo -op", -homo1 == HomoInt(-3, 1));
        OPT_CASE("hetero -op", -hetero1 == HeteroInt(3, -1));

        OPT_CASE("hetero *op", hetero1*3 == HeteroInt(-9, 3));
        // OPT_CASE("homo *op", homo1*5 == HomoInt(15, -5));
        OPT_CASE("hetero op*", 3*hetero1 == HeteroInt(-9, 3));
        // OPT_CASE("homo op*", 5*homo1 == HomoInt(15, -5));

        // homo1.applyEach([](int& coord){
        //     coord *= 10;
        // });
        // OPT_CASE("applyEach void", homo1 == HomoInt(30, -10));

    });

    // DModCoords aCoord(4, 5);
    // DModCoords bCoord(98, 99);
    // dlog("aCoord then:", aCoord);
    // dlog("bCoord then:", bCoord);
    // f<DAModInt, DModCoords>(GetFile<DAModInt, DDModInt>);
    // dlog("aCoord now:", aCoord);
    // dlog("bCoord now:", bCoord);

    TEST("parseSfenPos", {
        std::string testSfen = "(1)1rkr1(1)/2(3)2/(1)1RKR1(1)";
        // Test parseSfen by having it read and echo a sfen.
        std::string outputSfen = "";
        parseSfenPos(
            testSfen,
            [&](SquareEnum _piece) {
                outputSfen += getCharFromSquare(_piece);
            },
            [&](int _numVoid) {
                outputSfen += "(" + std::to_string(_numVoid) + ")";
            },
            [&](int _numEmpty) {
                outputSfen += std::to_string(_numEmpty);
            },
            [&]() {
                outputSfen += "/";
            }
        );
        REQ_CASE("echo sfen", outputSfen == testSfen);
    });

    std::cout << game.print() << std::endl;
    std::cout << "Done testing," << std::endl;
    if (TestMacros::numTestsFailed == 0) {
        std::cout << "Passed all " << TestMacros::numTestsPassed << " tests" << std::endl;
    } else {
        std::cout << "Failed " << TestMacros::numTestsFailed << " out of " << TestMacros::numTestsPassed + TestMacros::numTestsFailed << " tests." << std::endl;
    }
    return 0;


}