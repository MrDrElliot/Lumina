

#include <clang-c/Index.h>
#include <iostream>
#include <Lumina_eastl.cpp>
#include "Memory/Memory.h"
#include "Core/Application/ApplicationGlobalState.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Paths/Paths.h"
#include "Reflector/TypeReflector.h"


using namespace Lumina;

int main(int argc, char* argv[])
{
    FApplicationGlobalState GState;

    const char* LuminaDirectory = std::getenv("LUMINA_DIR");
    FString LuminaEditor(LuminaDirectory);
    LuminaEditor += "\\Lumina.sln";
    
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision( 2 );

    LOG_INFO("===============================================");
    LOG_INFO("Lumina Reflection Tool");
    LOG_INFO("===============================================");

    if (!std::filesystem::exists(LuminaEditor.c_str()))
    {
        std::cerr << "Failed to find Lumina.sln";
        return 0;
    }

    Reflection::FTypeReflector TypeReflector(LuminaEditor);

    
    {
        auto start = std::chrono::high_resolution_clock::now();

        if (!TypeReflector.ParseSolution())
        {
            LOG_ERROR("Failed to parse solution: {0}", LuminaEditor);
            return 0;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        LOG_INFO("Solution Parsing Took: {0} seconds", duration.count());
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();

        TypeReflector.Build();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        LOG_INFO("Build took: {0} seconds", duration.count());
    }
    
    return 0;
}



DECLARE_MODULE_ALLOCATOR_OVERRIDES()


namespace eastl
{
    void AssertionFailure(const char* expression)
    {
        // You can also hook this into your engine's log or crash reporter
        std::fprintf(stderr, "EASTL Assertion Failure: %s\n", expression);
        std::abort(); // Crash the program, like assert would
    }
}
