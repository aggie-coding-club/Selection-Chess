#include "array_board.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "game.h"
#include "min_max.h"

#include <iostream>
#include <iomanip>

const size_t MAX_MESSAGE = sizeof(size_t);

// global variable that gets set after a call to TEST macro. Returns true iff all TEST macro cases succeed.
bool testState;
// Conglomerate of cases. First param is name of the test, second param is a block of code, which contains CASE macros.
#define TEST(_test, _code) testState=true; printTestLabel(_test); do {_code} while (false);
// If a case fails, just set testState to false but continue with the next case.
#define OPT_CASE(_case, _bool) printTestCase(_case, _bool); testState=false;
// If a case fails, no more cases in that TEST are tried.
#define REQ_CASE(_case, _bool) OPT_CASE(_case, _bool); if (!_bool) break

void printTestLabel(std::string _test) {
    std::cout << "Running test '" << _test << "'" << std::endl;
}

void printTestCase(std::string _case, bool _condition) {
    std::cout << "   Case: " << std::setw(20) << std::setfill(' ') << std::left << 
        _case << " " << (_condition?"passed":">> FAILED <<") << std::endl;
}

int main() {
    std::cout << "Conducting tests..." << std::endl;

    TEST("meta1", {
        REQ_CASE("one", true);
        REQ_CASE("two", true);
        REQ_CASE("three", false);
        REQ_CASE("four", true);
        REQ_CASE("five", false);
    });
    TEST("meta2", {
        OPT_CASE("divide by 0", true);
        OPT_CASE("negative", true);
        OPT_CASE("overflow", false);
        OPT_CASE("underflow", true);
        OPT_CASE("copy ctor", false);
    });
    std::cout << "Done testing!" << std::endl;
    return 0;
}