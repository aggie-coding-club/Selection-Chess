#include "engine_runner.h"
#include "../chess/chess_utils.h"

#include <iostream>

#define edout std::cout << "[" << m_name << "] " //TODO: consider replacing with something similar to dlog.

// TODO: idk if these macros are too ugly, maybe there is a cleaner solution
#define parseIntSanity int value; if (!parseAsInt(featureValue, value)) { m_toEngineStream << "rejected " << featureName << " " << featureValue << std::endl; continue; }
#define parseBoolSanity bool value; if (!parseAsBool(featureValue, value)) { m_toEngineStream << "rejected " << featureName << " " << featureValue << std::endl; continue; }
#define parseStringSanity std::string value; if (!parseAsQuoted(featureValue, value)) { m_toEngineStream << "rejected " << featureName << " " << featureValue << std::endl; continue; }


// functions to parse command-line values. Return false if it is the wrong type.
bool parseAsInt(std::string _raw, int& _output) {
    if (_raw == "") return false;
    for(char c : _raw) {
        if (!isdigit(c)) {
            return false;
        }
    }
    _output = std::stoi(_raw);
    return true;
}

bool parseAsQuoted(std::string _raw, std::string& _output, char _frontDelim='"', char _backDelim='"') {
    if (_raw.length() < 2) return false;
    if (_raw.front() != _frontDelim || _raw.back() != _backDelim) {
        return false;
    }
    _output = _raw.substr(1, _raw.length()-2);
    return true;
}
bool parseAsBool(std::string _raw, bool& _output) {
    int result;
    if (!parseAsInt(_raw, result)) {
        return false;
    }
    if (result == 0 || result == 1) {
        _output = result;
        return true;
    }
    return false;
}

EngineRunner::EngineRunner(std::string _path) {
    m_enginePath = _path;
}

bool EngineRunner::init(std::string _path) {
    m_enginePath = _path;
    return init();
}
bool EngineRunner::init() {
    // Set the name like this until the engine can tell us its name
    m_name = m_enginePath;
    edout << "Creating boost child process..." << std::endl;
    bp::child c(m_enginePath, bp::std_in < m_toEngineStream, bp::std_out > m_fromEngineStream);
    m_alive = true;

    std::vector<char> m_engineOutputBuffer(4096);

    // bp::child c(m_enginePath,
    //     bp::std_in < m_toEngineStream,
    //     bp::std_out > asio::buffer(m_engineOutputBuffer), 
    //     m_ios);

    // We are using xboard protocol
    // processCommands("", true); // Can't do this without io timeouts
    m_toEngineStream << "xboard" << std::endl;
    m_toEngineStream << "protover 2" << std::endl;

    // ----------- Feature parsing ----------- //
    // TODO: listen for 'feature' commands for 2 seconds.
    // TODO: respond to feature commands with accepted or rejected
    // TODO: if the feature 'done=0' is set, increase timeout significantly. Can be recieved even before xboard and protover.
    // TODO: if feature 'done=1' is set, stop waiting and move on
    edout << "Waiting for features..." << std::endl;
    bool done = false;
    while(!done) {
        edout << "fetching response... " << std::endl;
        CmdTokenizer* cmd = processCommands("feature");
        cmd->next(); // pop 'feature'
        for(;;) {
            std::string featureName = cmd->next();
            if (featureName.front() == EOF) break;
            edout << "got featureName:" << featureName << std::endl;
            std::string equals = cmd->next();
            if (equals != "=") {
                edout << "Invalid feature format! Expected '=', got '" << equals << "'" << std::endl;
                break;
                //TODO: handle error
            }
            std::string featureValue = cmd->next();
            edout << "got featureValue:" << featureValue << std::endl;
            if (featureName == "done") {
                parseIntSanity // sanitizes featureValue to value & handles errors
                m_toEngineStream << "accepted " << featureName << std::endl;
                if (value == 1) { // Done with sending features
                    done = true;
                    break;
                } else {
                    edout << "WARNING: done=0 not yet implemented!" << std::endl;
                    // TODO: increase timeout
                }
            } else if (featureName == "debug") {
                parseBoolSanity
                m_toEngineStream << "accepted " << featureName << std::endl;
            } else if (featureName == "myname") {
                parseStringSanity
                m_name = value;
                m_toEngineStream << "accepted " << featureName << std::endl;
            } else {
                m_toEngineStream << "rejected " << featureName << std::endl;
            } //TODO: add all features
            edout << "done analyzing feature: " << featureName << "." << std::endl;
        }
    }

    m_toEngineStream << "new" << std::endl;
    // TODO: check engine actually can play selchess, return false otherwise.
    m_toEngineStream << "variant selchess" << std::endl;
    edout << "Done initializing" << std::endl;
    return true;
}


void EngineRunner::quit() {
        m_toEngineStream << "quit" << std::endl;
        // m_ios.run(); // blocking

        // std::cout << "Read buffer as: [";
        // for (char c: m_engineOutputBuffer) {
        //     std::cout << c;
        // }
        // std::cout << "]" << std::endl; 

        // int result = c.exit_code();
        m_alive = false;
    edout << "Done with boost code." << std::endl;
}

std::unique_ptr<Move> EngineRunner::getMove() {
    edout << "getMove" << std::endl;
    m_toEngineStream << "go" << std::endl;
    CmdTokenizer* cmd = processCommands("move");
    cmd->next(); // pop 'move'
    std::string moveString = cmd->next();
    return readAlgebraic(moveString); // TODO: sanity checks
}

bool EngineRunner::setMove(const Move& _move) {
    m_toEngineStream << _move.algebraic() << std::endl;
    return true;
}

CmdTokenizer* EngineRunner::getCommand() {
    // TODO: add timeouts. This is a pain in the ass since there is no portable way to do this. 
    // Maybe this will help? Idk if reading from pipe has magic properties that make this not applicable... 
    // https://www.boost.org/doc/libs/1_75_0/libs/beast/doc/html/beast/ref/boost__beast__basic_stream.html 
    // Update: an equivalent feature to timeouts is thread cancellation. Unfortunately, there is no portable
    // way to do this either. So if either feature can be added, the other can be built from it.
    std::string line;
    for (;;) {
        edout << "getting line..." << std::endl;
        if (!m_fromEngineStream || !std::getline(m_fromEngineStream, line)) {
            // break;
            edout << "engine died!" << std::endl;
            throw CustomException::ENGINE_DIED;
        }
        if (line.empty()) {
            edout << "got empty line from engine" << std::endl;
            // TODO: wait for input
            return nullptr;
        }
        edout << "got line " << line << std::endl;
        CmdTokenizer* tokenizer = new CmdTokenizer(line); //TODO: feels weird using the heap for this
        if(tokenizer->peek()[0] == '#') {
            // edout << "got comment from engine" << std::endl;
            // TODO: process comments
            continue;
        }
        return tokenizer;
    }
}

CmdTokenizer* EngineRunner::processCommands(std::string _cmdName, bool _ignore) {
    for (;;) {
        CmdTokenizer* cmd = getCommand(); // TODO: it's kinda weird CmdTokenizer is constructed here
        if (cmd == nullptr) {
            // reached end of commands buffer without finding our command
            return nullptr;
        }
        if (_cmdName != "") {
            edout << "checking if " << cmd->peek() << " == " << _cmdName << std::endl;
            if(cmd->peek() == _cmdName) {
                edout << "yah, so return cmd" << std::endl;
                return cmd;
            }
        }
        if (!_ignore) {
            // TODO: process other commands here
        }
        delete cmd;
    }
}

void EngineRunner::run() {

}

bool EngineRunner::setBoard(std::string _sfen) {

    m_toEngineStream << "setboard " << _sfen << std::endl;
    return true;
}
