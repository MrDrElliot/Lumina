#pragma once

#include <iostream>
#include <cstdlib>
#include "Platform/Platform.h"
#include <fstream>
#include "Log/Log.h"

#ifdef _MSC_VER
#include <intrin.h>    // for __debugbreak
#endif
#include <csignal> // for raise(SIGTRAP)

#ifdef LE_PLATFORM_WINDOWS
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

    // Initialize symbols for stack trace
    SymInitialize(process, NULL, TRUE);
    frames = CaptureStackBackTrace(0, 100, stack, NULL);

    symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;

    IMAGEHLP_LINE64 line;
    DWORD displacement;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    LOG_ERROR("=---------------------Dumping Callstack---------------------=");
    for (unsigned short i = 0; i < frames; ++i)
    {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

        if (SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &displacement, &line))
        {
            if (Lumina::FLog::IsInitialized())
            {
                LOG_ERROR("{0}: {1} - {2} (File: {3}, Line: {4})",
                          i, symbol->Name, symbol->Address, line.FileName, line.LineNumber);
            }
            else
            {
                std::cout << i << ": " << symbol->Name << " - " << symbol->Address
                          << " (File: " << line.FileName << ", Line: " << line.LineNumber << ")" << std::endl;
            }
        }
        else
        {
            if (Lumina::FLog::IsInitialized())
            {
                LOG_ERROR("{0}: {1} - {2} (No Line Info)", i, symbol->Name, symbol->Address);
            }
            else
            {
                std::cout << i << ": " << symbol->Name << " - " << symbol->Address << " (No Line Info)" << std::endl;
            }
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

// Macro for assertion, triggering a breakpoint if the condition fails
#define Assert(condition)                               \
    do {                                                \
        if ((!(condition))) [[unlikely]] {              \
            LOG_CRITICAL("Assertion failed: {0} in {1} at line {2}", \
                #condition, __FILE__, __LINE__);        \
            PrintCallStack();                           \
            /* Platform-specific debugging */            \
            __debugbreak();                             \
            std::exit(1);                               \
        }                                               \
    } while (false);

// Macro for assertion with a custom message
#define AssertMsg(condition, msg)                       \
    do {                                                \
        if ((!(condition))) [[unlikely]] {               \
            LOG_ERROR("Assertion failed: {0} ({1}) in {2} at line {3}", \
                msg, #condition, __FILE__, __LINE__);    \
            PrintCallStack();                           \
            /* Platform-specific debugging */            \
            __debugbreak();                             \
            std::exit(1);                               \
        }                                               \
    } while (false)

// Ensure macro (will log and sleep for a while on failure)
#define EnsureMsg(condition, msg)                       \
do {                                                    \
    if ((!(condition))) [[unlikely]] {                  \
        LOG_ERROR("Ensure failed: {0} ({1}) in {2} at line {3}", \
        msg, #condition, __FILE__, __LINE__);            \
        PrintCallStack();                                \
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); \
    }                                                   \
} while (false)

#define LUMINA_NO_ENTRY() AssertMsg(false, "No Entry function called!")
