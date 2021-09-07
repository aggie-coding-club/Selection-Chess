// // TODO: This is unexpectedly complex, and way too much effort rn just to get a nice debug print for the engine.
// class StreamCommenter {
//     public:
//     std::stringstream m_buffer;
//     StreamCommenter& operator<< (std::ostream&(*_manipulator)(std::ostream&)) {
        
//     }
//     StreamCommenter& operator<< (std::string _val) {
//         std::cout << "called string version" << std::endl;
//         m_buffer << std::regex_replace(_val, std::regex("a"), "ee");
//         return *this;
//     }
//     template<typename T>
//     StreamCommenter& operator<< (T _val) {
//         //TODO:
//         m_buffer << _val;
//         return *this;
//     }
// };
// void operator<< (std::ostream& _os, StreamCommenter& _sc);


// dout macro 'debug output'
// like cout, cerr, etc., except only outputs to console/log in debug mode.
extern std::ostream g_nullout;
#ifdef DEBUG
    #if DEBUG & (1<<0)
        #define dout std::cout
    #else 
        #define dout g_nullout
    #endif
    #if DEBUG & (1<<1)
        #define tdout std::cout
    #else 
        #define tdout g_nullout
    #endif
#else
    #define dout g_nullout
    #define tdout g_nullout
#endif
