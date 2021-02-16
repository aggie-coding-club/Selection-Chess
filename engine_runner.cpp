#include <iostream>

#include <boost/process.hpp> // note: requires boost >= 1_64_0
// Also note, if you are using VSCode, add your boost path as an includePath to your c_cpp_properties.json if your Intelisense isn't recognizing it
#include <boost/asio.hpp>

namespace bp = boost::process;
namespace asio = boost::asio;

#include "engine_runner.h"

EngineRunner::EngineRunner(std::string _path) {
    std::cout << "Running simpleEcho.cpp..." << std::endl;

    std::cout << "Running boost code with path '" << _path << "'..." << std::endl;
        boost::asio::io_service ios;
        std::vector<char> bufOut(4096); // FIXME: can overflow
        bp::opstream outStream;

        bp::child c(_path,
         bp::std_in < outStream,
         bp::std_out > asio::buffer(bufOut), 
         ios);

         outStream << "lol" << std::endl;
         outStream << "quit" << std::endl;

        ios.run(); // blocking

        std::cout << "Read buffer as: [";
        for (char c: bufOut) {
            std::cout << c;
        }
        std::cout << "]" << std::endl; 

        int result = c.exit_code();
    std::cout << "Done with boost code." << std::endl;

}
