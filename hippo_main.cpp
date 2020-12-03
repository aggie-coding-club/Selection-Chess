#include <iostream>

#include "constants.h"

// TODO: async search tree functions
const size_t MAX_MESSAGE = sizeof(size_t);

int main() {
    std::cout << "Yes this is hippo " << std::endl;
    std::string inp = "NOTHING READ.";
    std::cin >> inp;
    std::cout << "Hippo read [" << std::flush << inp << "]" << std::endl;

    // init shared variables, mutexes, etc.

    // start interface thread

    while (true) { //FIXME: stop it, get some help (ugly busy waiting)
        if (std::cin >> inp) {
            std::cout << "Hippo read [" << inp << "]" << std::endl;
            if (inp == "quit") {
                break;
            }
        }
    }
    std::cout << "Hippo says goodbye." << std::endl;
    return 0;
}

