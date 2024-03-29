#include "human_runner.h"
#include "../chess/chess_utils.h"

#include <iostream>

#define edout std::cout << "[" << m_name << "] " //FIXME

HumanRunner::HumanRunner(std::string _name) {
    m_name = _name;
}
bool HumanRunner::init() {
    m_alive = true;
    edout << "done initializing" << std::endl;
    return true;
}

void HumanRunner::quit() {
    m_alive = false;
}

std::unique_ptr<Move> HumanRunner::getMove() {
    std::cout << "Human, tell me your move: " << std::flush;
    std::string humanMove;
    std::cin >> humanMove;
    std::cout << "Thanks, human." << std::endl;
    return readAlgebraic(humanMove);
}

bool HumanRunner::setMove(const Move& _move) {
    edout << "recieved move " << _move.algebraic() << std::endl;
    return true;
}


void HumanRunner::run() {

}

bool HumanRunner::setBoard(std::string _sfen) {
    edout << "setboard(" << _sfen << ")" << std::endl;
    return true; // TODO
}
