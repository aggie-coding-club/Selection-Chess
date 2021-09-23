#include "game.h"
#include "constants.hpp"
#include "chess_utils.h"
#include "array_board.h"

Game::Game(const std::string _sfen, std::string _rulesFile) : m_rules(_rulesFile) {
    reset(_sfen);
}

void Game::reset(std::string _sfen) {
    BoardPrintSettings ps;
    if (m_board != nullptr) { // Copy over the print settings of the last board, if available
        ps = m_board->m_printSettings;
    }
    auto fields = split(_sfen, "\\s+");
    m_board = new ArrayBoard(m_rules, fields[0]);
    m_board->m_printSettings = ps;
    // TODO: cleanly handle all these errors
    if (fields.size() == 4) { // whole sfen is given
        dlog("got 4 fields");
        if (fields[1] == "w") {
            m_turn = WHITE;
        } else if (fields[1] == "b") {
            m_turn = BLACK;
        } else {
            dlog("ERROR! unknown color [",fields[1],"] in SFEN [", _sfen,"]");
        }

        // TODO: check these are actually numbers
        m_50Count = std::stoi(fields[2]);
        m_moveCount = std::stoi(fields[3]);
    } else if (fields.size() == 1) { // if given just position, we'll just assume this is start of new game
        m_turn = WHITE;
        m_50Count = 0;
        m_moveCount = 1;
    } else { 
        dlog("ERROR! invalid number of fields in SFEN [", _sfen, "]");
    }
    dlog("Game Constructed board");
}

std::string Game::toSfen() const {
    return m_board->toSfen() + " " + 
    (m_turn == WHITE? "w" : "b") + " " +
    std::to_string(m_50Count) + " " +
    std::to_string(m_moveCount);
}


std::string Game::print() {
    std::string result = m_board->getAsciiBoard();
    result += m_board->toSfen() + "\n";
    if (!m_moveHistory.empty()) {
        result += "Last move: " + m_moveHistory.top()->algebraic() + "\n";
    }
    result += "Moves since last capture: " + std::to_string(m_50Count) + "\n";
    result += "Estimated score for this position: " + std::to_string(m_board->staticEvaluation()) + "\n";
    result += m_turn == WHITE? "White's Turn\n" : "Black's Turn\n";
    return result;
}

bool Game::applyMove(std::shared_ptr<Move> _move){
    if (!m_board->apply(_move)) {
        return false;
    }
    // increment full move count when black plays
    if (m_turn == BLACK) {
        m_moveCount++;
    }
    m_turn = -m_turn;
    m_moveHistory.push(_move);
    if (_move->m_type == PIECE_MOVE && (std::static_pointer_cast<PieceMove>(_move))->m_capture != EMPTY) {
        m_50Count = 0;
    } else {
        m_50Count++;
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
        // decrement full move count when undoing black's move
        if (m_turn == BLACK) {
            m_moveCount--;
        }
        // TODO: how do we restore moveSinceLastCapture?
    }
    return true;
};
