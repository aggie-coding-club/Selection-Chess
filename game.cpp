#include "constants.h"
#include "utils.h"

#include "game.h"
#include "array_board.h"
#include "dll_board.h"

Game::Game(const std::string _sfen) {
    // TODO: implement
    // m_board = new ArrayBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    m_board = new DLLBoard(_sfen);
    dout << "Game Constructed board" << std::endl;
}

std::string Game::print() {
    return m_board->getAsciiBoard();
}