#include "min_max.h"

#include <iostream>
#include <algorithm>

std::pair<int,Move*> minmax(Game* _game, int _depth, std::string& _history) {
    if (_depth == 0) {// or if in stale/checkmate.
        int value = _game->m_board->staticEvaluation();
        return std::make_pair(value, nullptr);
    }

    std::vector<Move*> legalMoves = _game->m_board->getMoves(_game->m_turn);
    std::string depthPadding = std::string(_depth-1, '\t');
    if (_game->m_turn == WHITE) {
        if (legalMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
            return std::make_pair(-PIECE_VALUES[W_KING], nullptr);
        }
        auto bestResult = std::make_pair(std::numeric_limits<int>::min(), (Move*)nullptr);
        for (Move* m : legalMoves) {
            _game->applyMove(m);
            auto result = minmax(_game, _depth-1, _history);
            _history += depthPadding + std::to_string(result.first) + " " + m->algebraic() + '\n';
            _game->undoMove(1);
            if (result.first > bestResult.first) { // white tries to maximize
                bestResult.first = result.first;
                bestResult.second = m;
            }
        }
        return bestResult;
    } else { // turn is black's
        if (legalMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
            return std::make_pair(-PIECE_VALUES[B_KING], nullptr);
        }
        auto bestResult = std::make_pair(std::numeric_limits<int>::max(), (Move*)nullptr);
        for (Move* m : legalMoves) {
            _game->applyMove(m);
            auto result = minmax(_game, _depth-1, _history);
            _history += depthPadding + std::to_string(result.first) + " " + m->algebraic() + '\n';
            _game->undoMove(1);
            if (result.first < bestResult.first) { // black tries to minimize
                bestResult.first = result.first;
                bestResult.second = m;
            }
        }
        return bestResult;
    }
}

std::pair<int,Move*> negamax(Game* _game, int _depth) {
    if (_depth == 0) {// or if in stale/checkmate.
        int value = _game->m_board->staticEvaluation();
        value *= _game->m_turn;
        return std::make_pair(value, nullptr);
    }

    std::vector<Move*> legalMoves = _game->m_board->getMoves(_game->m_turn);

    if (legalMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
        return std::make_pair(-PIECE_VALUES[W_KING], nullptr);
    }

    auto bestResult = std::make_pair(std::numeric_limits<int>::min(), (Move*) nullptr);
    for (Move* m : legalMoves) {
        _game->applyMove(m);
        auto result = negamax(_game, _depth-1);
        result.first = - result.first; // Benefit to our opponent is our detriment
        _game->undoMove(1);
        if (result.first > bestResult.first) { // maximize our benefit
            bestResult.first = result.first;
            bestResult.second = m;
        }
    }
    return bestResult;
}

std::pair<int,Move*> negamaxAB(Game* _game, int _depth, int _alpha, int _beta) {
    if (_depth == 0) {// or if in stale/checkmate.
        int value = _game->m_board->staticEvaluation();
        value *= _game->m_turn;
        return std::make_pair(value, nullptr);
    }

    std::vector<Move*> legalMoves = _game->m_board->getMoves(_game->m_turn);

    if (legalMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
        return std::make_pair(-PIECE_VALUES[W_KING], nullptr);
    }

    auto bestResult = std::make_pair(std::numeric_limits<int>::min(), (Move*) nullptr);
    for (Move* m : legalMoves) {
        _game->applyMove(m);
        auto result = negamaxAB(_game, _depth-1, -_beta, -_alpha);
        result.first = - result.first; // Benefit to our opponent is our detriment
        _game->undoMove(1);
        if (result.first > bestResult.first) { // maximize our benefit
            bestResult.first = result.first;
            bestResult.second = m;
        }
        _alpha = std::max(_alpha, bestResult.first);
        if (_alpha >= _beta) {
            break;
        }
    }
    return bestResult;
}
