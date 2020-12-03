#ifdef _WIN32
    #include "engine_runner_windows.cpp"
#endif

#ifdef linux
    #include "engine_runner_posix.cpp"
#endif