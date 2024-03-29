#include "min_max.h"

#include <iostream>
#include <algorithm>

std::pair<int,std::unique_ptr<Move>> minmax(Game* _game, int _depth, std::string& _history) { //FIXME: remove minmax, maintaining repetive code is annoying
    if (_depth == 0) {// or if in stale/checkmate.
        int value = _game->m_board->staticEvaluation();
        return std::make_pair(value, std::unique_ptr<Move>(nullptr));
    }

    std::vector<std::unique_ptr<Move>> maybeMoves = _game->m_board->getMaybeMoves(_game->m_turn);
    std::string depthPadding = std::string(_depth-1, '\t');
    if (_game->m_turn == WHITE) {
        if (maybeMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
            return std::make_pair(-PIECE_VALUES[W_KING], std::unique_ptr<Move>(nullptr));
        }
        auto bestResult = std::make_pair(std::numeric_limits<int>::min(), std::unique_ptr<Move>(nullptr));
        for (std::unique_ptr<Move>& move : maybeMoves) {
            if(!_game->applyMove(std::move(move))) { // Careful, we give up ownership of move here
                // Turns out this move wasn't legal, whoopsie
                // tdout << "skipping move " << move->algebraic() << " because it wasn't actually legal." << std::endl;
                continue; // FIXME: this means checking maybeMoves.size() doesn't detect stalemates
            }
            auto result = minmax(_game, _depth-1, _history);
            move = _game->undoMove(); // get ownership of move back
            _history += depthPadding + std::to_string(result.first) + " " + move->algebraic() + '\n';
            if (move == nullptr) {
                std::cerr << "Board is corrupted, exiting!" << WHERE << std::endl;
                exit(EXIT_FAILURE);
            }
            if (result.first > bestResult.first) { // white tries to maximize
                bestResult.first = result.first;
                bestResult.second = std::move(move);
            }
        }
        return bestResult;
    } else { // turn is black's
        if (maybeMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
            return std::make_pair(-PIECE_VALUES[B_KING], std::unique_ptr<Move>(nullptr));
        }
        auto bestResult = std::make_pair(std::numeric_limits<int>::max(), std::unique_ptr<Move>(nullptr));
        for (std::unique_ptr<Move>& move : maybeMoves) {
            if(!_game->applyMove(std::move(move))) {
                // Turns out this move wasn't legal, whoopsie
                // tdout << "skipping move " << move->algebraic() << " because it wasn't actually legal." << std::endl;
                continue;
            }
            auto result = minmax(_game, _depth-1, _history);
            move = _game->undoMove();
            _history += depthPadding + std::to_string(result.first) + " " + move->algebraic() + '\n';
            if (move == nullptr) {
                std::cerr << "Board is corrupted, exiting!" << WHERE << std::endl;
                exit(EXIT_FAILURE);
            }
            if (result.first < bestResult.first) { // black tries to minimize
                bestResult.first = result.first;
                bestResult.second = std::move(move);
            }
        }
        return bestResult;
    }
}

std::pair<int,std::unique_ptr<Move>> negamax(Game* _game, int _depth) {
    if (_depth == 0) {// or if in stale/checkmate.
        int value = _game->m_board->staticEvaluation();
        value *= _game->m_turn;
        return std::make_pair(value, std::unique_ptr<Move>(nullptr));
    }

    std::vector<std::unique_ptr<Move>> maybeMoves = _game->m_board->getMaybeMoves(_game->m_turn);

    if (maybeMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
        return std::make_pair(-PIECE_VALUES[W_KING], std::unique_ptr<Move>(nullptr));
    }

    auto bestResult = std::make_pair(std::numeric_limits<int>::min(), std::unique_ptr<Move>(nullptr));
    for (std::unique_ptr<Move>& move : maybeMoves) {
        if(!_game->applyMove(std::move(move))) {
            // Turns out this move wasn't legal, whoopsie
            // tdout << "skipping move " << move->algebraic() << " because it wasn't actually legal." << std::endl;
            continue;
        }
        auto result = negamax(_game, _depth-1);
        result.first = - result.first; // Benefit to our opponent is our detriment

        move = _game->undoMove();
        if (move == nullptr) {
            std::cerr << "Board is corrupted, exiting!" << WHERE << std::endl; // TODO: would be nice to have cleaner crash
            exit(EXIT_FAILURE);
        }
        if (result.first > bestResult.first) { // maximize our benefit
            bestResult.first = result.first;
            bestResult.second = std::move(move);
        }
    }
    return bestResult;
}

std::pair<int,std::unique_ptr<Move>> negamaxAB(Game* _game, int _depth, int _alpha, int _beta) {
    if (_depth == 0) {// or if in stale/checkmate.
        int value = _game->m_board->staticEvaluation();
        value *= _game->m_turn;
        return std::make_pair(value, std::unique_ptr<Move>(nullptr));
    }

    std::vector<std::unique_ptr<Move>> maybeMoves = _game->m_board->getMaybeMoves(_game->m_turn);

    if (maybeMoves.size() == 0) { // FIXME: temporary hack to handle stalemates
        return std::make_pair(-PIECE_VALUES[W_KING], std::unique_ptr<Move>(nullptr));
    }

    auto bestResult = std::make_pair(std::numeric_limits<int>::min(), std::unique_ptr<Move>(nullptr));
    for (std::unique_ptr<Move>& move : maybeMoves) {
        if(!_game->applyMove(std::move(move))) {
            // Turns out this move wasn't legal, whoopsie
            // tdout << "skipping move " << move->algebraic() << " because it wasn't actually legal." << std::endl;
            continue;
        }
        auto result = negamaxAB(_game, _depth-1, -_beta, -_alpha);
        result.first = - result.first; // Benefit to our opponent is our detriment

        move = _game->undoMove();
        if (move == nullptr) {
            std::cerr << "Board is corrupted, exiting!" << WHERE << std::endl;
            exit(EXIT_FAILURE);
        }
        if (result.first > bestResult.first) { // maximize our benefit
            bestResult.first = result.first;
            bestResult.second = std::move(move);
        }
        _alpha = std::max(_alpha, bestResult.first);
        if (_alpha >= _beta) {
            break;
        }
    }
    return bestResult;
}
