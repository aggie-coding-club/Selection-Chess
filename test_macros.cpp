#include "test_macros.h"

namespace TestMacros {
    bool testState;
    bool tempBoolForCases;
    std::string failedCheckName;
    bool skipRemCases;
    int numTestsFailed;
    int numTestsPassed;

    void printTestLabel(std::string _test) {
        std::cout << "Running test '" << _test << "'" << std::endl;
    }

    void printTestCase(std::string _case, bool _condition) {
        std::cout << "   Case: " << std::setw(20) << std::setfill(' ') << std::left << _case << " "; 
        if (_condition) {
            std::cout << "passed" << std::endl;
        } else {
            std::cout << ">> FAILED <<";
            if (failedCheckName != "") {
                std::cout << " on check '" << failedCheckName << "'";
            }
            std::cout << std::endl;
        }
    }
    void printSkippedCase(std::string _case) {
        std::cout << "   Case: " << std::setw(20) << std::setfill(' ') << std::left << _case << " "; 
        std::cout << "[skipped]" << std::endl;
    }
}