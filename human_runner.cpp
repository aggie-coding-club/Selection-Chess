#include <iostream>

#include "human_runner.h"
#include "utils.h"

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

Move HumanRunner::getMove() {
    edout << "getMove" << std::endl;
    //TODO: literally get move from human
    return Move();
}

bool HumanRunner::setMove(Move _move) {
    return true;
}


void HumanRunner::run() {

}