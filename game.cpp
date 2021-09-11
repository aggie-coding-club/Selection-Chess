#include "game.h"
#include "constants.h"
#include "utils.h"
#include "array_board.h"

Game::Game(const std::string _sfen, std::string _rulesFile) : m_rules(_rulesFile) {
    reset(_sfen);
}

void Game::reset(const std::string _sfen) {
    BoardPrintSettings ps;
    if (m_board != nullptr) {
        ps = m_board->m_printSettings;
    }
    // TODO: implement
    // m_board = new ArrayBoard("rnbqkbnr/pppppppp/8/2(4)2/(2)4/18/PPPPPPPP/RNBQKBNR w 0 1");
    m_board = new ArrayBoard(m_rules, _sfen);
    m_board->m_printSettings = ps;
    // tdout << "Game Constructed board" << std::endl;
}

std::string Game::print() {
    std::string result = m_board->getAsciiBoard();
    result += m_board->toSfen() + "\n";
    if (!m_moveHistory.empty()) {
        result += "Last move: " + m_moveHistory.top()->algebraic() + "\n";
    }
    result += "Moves since last capture: " + std::to_string(m_movesSinceLastCapture) + "\n";
    result += "Estimated score for this position: " + std::to_string(m_board->staticEvaluation()) + "\n";
    result += m_turn == WHITE? "White's Turn\n" : "Black's Turn\n";
    return result;
}

bool Game::applyMove(std::shared_ptr<Move> _move){
    if (!m_board->apply(_move)) {
        return false;
    }
    m_turn = -m_turn;
    m_moveHistory.push(_move);
    if (_move->m_type == PIECE_MOVE && (std::static_pointer_cast<PieceMove>(_move))->m_capture != EMPTY) {
        m_movesSinceLastCapture = 0;
    } else {
        m_movesSinceLastCapture++;
    }
    return true;
};

bool Game::undoMove(size_t _numMoves) {
    for (; _numMoves > 0; _numMoves--) {
        std::shared_ptr<Move> undone = m_moveHistory.top();
        if (!m_board->undo(undone)) {
            return false;
        }
        m_moveHistory.pop();
        m_turn = -m_turn;
        // TODO: how do we restore moveSinceLastCapture?
    }
    return true;
};
