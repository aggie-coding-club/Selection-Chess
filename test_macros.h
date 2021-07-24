#ifndef TEST_MACROS_H
#define TEST_MACROS_H

// TODO: rename this file

#include <iostream>
#include <iomanip>


namespace TestMacros {
    // global variable that gets set after a call to TEST macro. Returns true iff all TEST macro cases succeed.
    extern bool testState;

    // Where result of bool is stored for case macros, to avoid multiple calls if a non-const bool function is given or anything else wacky.
    extern bool tempBoolForCases;

    // Name of the specific check of a case that failed, if using a multi-check case.
    extern std::string failedCheckName;

    void printTestLabel(std::string _test);
    void printTestCase(std::string _case, bool _condition);
}

// Conglomerate of cases. First param is name of the test, second param is a block of code, which contains CASE macros.
// Sets testState to true iff all of its cases pass. Stops early if a REQ_CASE is failed.
// Note that this macro is kinda abusive of the `break` keyword, so do not use loops or switch statements inside the _code param. //TODO: this should be fixed once 'skipped' prints are shown.
#define TEST(_test, _code) \
    TestMacros::testState=true;\
    TestMacros::printTestLabel(_test);\
    do {_code} while (false)

// If this CASE fails, just set testState to false but continue with the next CASE.
#define OPT_CASE(_case, _bool) \
    TestMacros::failedCheckName = "";\
    TestMacros::tempBoolForCases=(_bool);\
    TestMacros::printTestCase(_case, TestMacros::tempBoolForCases);\
    TestMacros::testState &= TestMacros::tempBoolForCases

// If this CASE fails, no more CASEs in that TEST are tried.
#define REQ_CASE(_case, _bool) \
    OPT_CASE(_case, _bool);\
    if (!TestMacros::tempBoolForCases) break

// Given as the _bool param of a CASE, will return true iff all CHECKs in _code pass. Stops at the first failed CHECK.
#define MULTI_CHECK(_code) \
        [&](){\
            _code\
            return true;\
        }()

// Note that this _checkName is only printed if this check causes the case to fail.
#define CHECK(_checkName, _bool) \
        if (!(_bool)) {\
            TestMacros::failedCheckName = _checkName;\
            return false;\
        }

#endif