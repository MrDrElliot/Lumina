#pragma once


#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <thread> // Include for std::this_thread::sleep_for
#include <chrono> // Include for std::chrono::milliseconds
#include "Log/Log.h"

#if LE_PLATFORM_WINDOWS
#include <windows.h>
#include <dbghelp.h>
#endif

inline void PrintCallStack()   
{
#if LE_PLATFORM_WINDOWS
    void* stack[100];
    unsigned short frames;
    SYMBOL_INFO* symbol;
    HANDLE process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);
    frames = CaptureStackBackTrace(0, 100, stack, NULL);
    symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;

    LOG_ERROR("=---------------------Dumping Callstack---------------------=");
    for (unsigned short i = 0; i < frames; ++i)
    {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        if(Lumina::FLog::IsInitialized())
        {
            LOG_ERROR("{0}: {1} - {2}", i, symbol->Name, symbol->Address);
        }
        else
        {
            std::cout << i << ": " << symbol->Name << " - " << symbol->Address << std::endl;
        }
    }
    LOG_ERROR("=---------------------End of CallStack---------------------=");


    free(symbol);
#endif
}

#if LE_PLATFORM_WINDOWS

inline LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* exceptionInfo)
{
    std::ofstream logFile("Logs/crash_log.txt", std::ios::app);
    logFile << "Unhandled Exception Occurred!" << std::endl;
    PrintCallStack();
    logFile.close();
    
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

#define Assert(condition)                               \
    do {                                               \
        if (!(condition)) {                            \
            LOG_CRITICAL("Assertion failed: {0} in {1} at line {2}", \
                #condition, __FILE__, __LINE__);     \
            PrintCallStack(),                        \
            std::abort();                             \
        }                                              \
    } while (false)

#define AssertMsg(condition, msg)                      \
    do {                                               \
        if (!(condition)) {                            \
            LOG_ERROR("Assertion failed: {0} ({1}) in {2} at line {3}", \
                msg, #condition, __FILE__, __LINE__);            \
            PrintCallStack(),                          \
            std::abort();                             \
        }                                              \
    } while (false)


#define ensureMsg(condition, msg)                      \
do {                                               \
    if (!(condition)) {                            \
        LOG_ERROR("Ensure failed: {0} ({1}) in {2} at line {3}", \
        msg, #condition, __FILE__, __LINE__);            \
        PrintCallStack();  \
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); \
    }                                              \
} while (false)