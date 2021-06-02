#include "human_runner.h"
#include "utils.h"

#include <iostream>

#define edout dout << "[" << m_name << "] "

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
    edout << "getMove" << std::endl;
    //TODO: literally get move from human
    return std::unique_ptr<Move>(nullptr);
}

bool HumanRunner::setMove(std::unique_ptr<Move>& _move) {
    return true;
}


void HumanRunner::run() {

}