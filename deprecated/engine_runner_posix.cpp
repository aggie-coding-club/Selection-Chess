// ----------- DEPRECATED FILE ----------- //
// ----------- Now using Boost ----------- //

#include <cstdint>
#include <stack>

#include "constants.h"
#include "utils.h"

#include "engine_runner.h"

#include "constants.h"
#include "engine_runner.h"


#include <windows.h> 
#include <winbase.h>
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#include <string>
#include <iostream>

#define BUFSIZE 4096 
 
HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
 
void CreateChildProcess(void); 
void WriteToPipe(std::string _toWrite);
void ReadFromPipe(void); 

bool Write(std::string, std::string&, DWORD, DWORD);

int _tmain(int argc, TCHAR *argv[]) 
{ 
   SECURITY_ATTRIBUTES saAttr; 
 
   printf("\n->Start of parent execution.\n");

// Set the bInheritHandle flag so pipe handles are inherited. 
 
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 

// Create a pipe for the child process's STDOUT. 
 
   if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) {
    std::cerr << "StdoutRd CreatePipe\n" << WHERE << std::endl;
      exit(EXIT_FAILURE);
   }

// Ensure the read handle to the pipe for STDOUT is not inherited.

   if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) ) {
    std::cerr << "Stdout SetHandleInformation\n" << WHERE << std::endl;
      exit(EXIT_FAILURE);
   }

// Create a pipe for the child process's STDIN. 
 
   if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
    std::cerr << "Stdin CreatePipe\n" << WHERE << std::endl;
      exit(EXIT_FAILURE);
   }

// Ensure the write handle to the pipe for STDIN is not inherited. 
 
   if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) ) {
    std::cerr << "Stdin SetHandleInformation\n" << WHERE << std::endl;
      exit(EXIT_FAILURE);
   }
 
// Create the child process. 
   
   CreateChildProcess();
 
// Write to the pipe that is the standard input for a child process. 
// Data is written to the pipe's buffers, so it is not necessary to wait
// until the child process is running before writing data.
 
   WriteToPipe("TEST\tOUTPUT PARAMETER\nhello\nhelp"); 

// Close the pipe handle so the child process stops reading. 
 
   if ( ! CloseHandle(g_hChildStd_IN_Wr) ) {
    std::cerr << "StdInWr CloseHandle\n" << WHERE << std::endl;
      exit(EXIT_FAILURE);
   }
    std::cout << "WriteToPipe finished" << std::endl;
 
// Read from pipe that is the standard output for child process. 
 
   printf( "\n->Contents of child process STDOUT:\n\n");
   ReadFromPipe(); 

   printf("\nNow let's get funky\n");
   std::string response;
   Write("uci", response, 0, 5000);
   std::cout << "Got the second response as {" << response << "}" << std::endl;

   Write("go abc 123", response, 0, 5000);
   std::cout << "Got the third response as {" << response << "}" << std::endl;

   Write("quit", response, 0, 5000);
   std::cout << "Got the last response as {" << response << "}" << std::endl;

   printf("\n->End of parent execution.\n");

// The remaining open handles are cleaned up when this process terminates. 
// To avoid resource leaks in a larger application, close handles explicitly. 

   return 0; 
} 
 
void CreateChildProcess()
// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{ 
   TCHAR szCmdline[]=TEXT("engines/Hippocrene.exe"); //TODO: magic string should come from somewhere
   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE; 
 
// Set up members of the PROCESS_INFORMATION structure. 
 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
// Set up members of the STARTUPINFO structure. 
// This structure specifies the STDIN and STDOUT handles for redirection.
 
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
// Create the child process. 
    
   bSuccess = CreateProcess(NULL, 
      szCmdline,     // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
   // If an error occurs, exit the application. 
   if ( ! bSuccess ) {
      std::cerr << "CreateProcess\n" << WHERE << std::endl;
      exit(EXIT_FAILURE);
   }
   else 
   {
      // Close handles to the child process and its primary thread.
      // Some applications might keep these handles to monitor the status
      // of the child process, for example. 

      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
      
      // Close handles to the stdin and stdout pipes no longer needed by the child process.
      // If they are not explicitly closed, there is no way to recognize that the child process has ended.
      
      CloseHandle(g_hChildStd_OUT_Wr);
      CloseHandle(g_hChildStd_IN_Rd);
   }
}

// Write to the engine and get a response. Return false if timeout or error.
bool Write(std::string _toWrite, std::string &_response, DWORD _writeTimeout, DWORD _readTimeout) {
    DWORD dwRead, dwWritten; 
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    std::cout << "copying _toWrite" << std::endl;
    strncpy(chBuf, _toWrite.c_str(), BUFSIZE); // FIXME: handle _toWrite longer than buffer
    std::cout << "straight cout is (" << chBuf << ")" << std::endl;

    // Set timeout data structure. // TODO: see if there is any performance or design issues with setting this each time.
    COMMTIMEOUTS CommTimeouts { // All values below are in milliseconds or ms/byte.
        0, // ReadIntervalTimeout = Max time allowed after retrieval of one byte before the next byte. 0 means not used.
        0, // ReadTotalTimeoutMultiplier = Additional time granted per byte we intend to read in.
        _readTimeout, // ReadTotalTimeoutConstant = normal timeout used when reading
        0, // WriteTotalTimeoutMultiplier = like ReadTotalTimeoutMultiplier, but for reading
        _writeTimeout  // WriteTotalTimeoutConstant = normal timeout used when writing; Zero means not used, so no timeout.
    };
    if (!SetCommTimeouts(g_hChildStd_IN_Wr, &CommTimeouts)) {
        std::cerr << "Unable to set timeout!" << WHERE << std::endl;
        return false;
    }

    std::cout << "writing to file {" << std::endl;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), chBuf, _toWrite.length(), &dwWritten, NULL); // debugging
    std::cout << "}" << std::endl;
    bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, _toWrite.length(), &dwWritten, NULL);
    if (!bSuccess) {
        std::cerr << "WRITE TO SUBPROCESS FAILED\n" << WHERE << std::endl;
        return false;
    }

    for (;;) {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if( ! bSuccess || dwRead == 0 ) break;
    }
    _response = std::string(chBuf); // Convert to the more user friendly c++ string. This is out actual output.
    return true;
}

void WriteToPipe(std::string _toWrite)
// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data. 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE];
   BOOL bSuccess = FALSE;
    std::cout << "copying _toWrite" << std::endl;
   strncpy(chBuf, _toWrite.c_str(), BUFSIZE); // FIXME: handle _toWrite longer than buffer
   std::cout << "straight cout is (" << chBuf << ")" << std::endl;

    std::cout << "writing to file {" << std::endl;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), chBuf, _toWrite.length(), &dwWritten, NULL); // debugging
    std::cout << "}" << std::endl;
    bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, _toWrite.length(), &dwWritten, NULL);
    if (!bSuccess) {
        std::cerr << "WRITE TO SUBPROCESS FAILED\n" << WHERE << std::endl;
    }
} 
 
void ReadFromPipe(void) 

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set timeout data structure. // TODO: see if there is any performance or design issues with setting this each time.
    // COMMTIMEOUTS CommTimeouts { // All values below are in milliseconds or ms/byte.
    //     0, // ReadIntervalTimeout = Max time allowed after retrieval of one byte before the next byte. 0 means not used.
    //     1, // ReadTotalTimeoutMultiplier = Additional time granted per byte we intend to read in.
    //     1000, // ReadTotalTimeoutConstant = normal timeout used when reading
    //     0, // WriteTotalTimeoutMultiplier = like ReadTotalTimeoutMultiplier, but for reading
    //     1000  // WriteTotalTimeoutConstant = normal timeout used when writing; Zero means not used, so no timeout.
    // };
    COMMTIMEOUTS CommTimeouts = {0}; 
    CommTimeouts.ReadIntervalTimeout = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 100;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 100;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;

    std::cout << "Am I about to crash here? at " << WHERE << "My last error was " << GetLastError();
    SetLastError(0);
    std::cout << "But is now 0" << std::endl;
    if (!SetCommTimeouts(g_hChildStd_OUT_Rd, &CommTimeouts)) {
        std::cerr << "Unable to set timeout!" << WHERE << "With this error code: " << GetLastError() << std::endl;
    }
    std::cout << "Nah homie we good" << std::endl;

   for (;;) 
   { 
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
      if( ! bSuccess || dwRead == 0 ) break; 
      std::cout << "dwRead: " << dwRead << std::endl;

      bSuccess = WriteFile(hParentStdOut, chBuf, 
                           dwRead, &dwWritten, NULL);
      if (! bSuccess ) break; 
   } 
   std::cout << "at " << WHERE << std::endl;
} 
