#include "game.h"
#include "constants.h"
#include "utils.h"
#include "array_board.h"
#include "dll_board.h"

Game::Game(const std::string _sfen) {
    reset(_sfen);
}

void Game::reset(const std::string _sfen) {
    BoardPrintSettings ps;
    if (m_board != nullptr) {
        ps = m_board->m_printSettings;
    }
    // TODO: implement
    // m_board = new ArrayBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    m_board = new DLLBoard(_sfen);
    m_board->m_printSettings = ps;
    dout << "Game Constructed board" << std::endl;
}

std::string Game::print() {
    std::string result = m_board->getAsciiBoard();
    if (!m_moveHistory.empty()) {
        result += "Last move: " + m_moveHistory.top().algebraic() + "\n";
    }
    result += "Moves since last capture: " + std::to_string(m_movesSinceLastCapture) + "\n";
    result += "Estimated score for this position: " + std::to_string(m_board->staticEvaluation()) + "\n";
    result += m_turnWhite? "White's Turn\n" : "Black's Turn\n";
    return result;
}

bool Game::applyMove(Move _move){
    if (!m_board->apply(_move)) {
        return false;
    }
    m_turnWhite = !m_turnWhite;
    m_moveHistory.push(_move);
    if (_move.m_capture != EMPTY) {
        m_movesSinceLastCapture = 0;
    } else {
        m_movesSinceLastCapture++;
    }
};

bool Game::undoMove(size_t _numMoves) {
    for (; _numMoves > 0; _numMoves--) {
        Move undone = m_moveHistory.top();
        if (!m_board->undo(undone)) {
            return false;
        }
        m_turnWhite = !m_turnWhite;
        // TODO: how do we restore moveSinceLastCapture?
        m_moveHistory.pop();
    }
};
