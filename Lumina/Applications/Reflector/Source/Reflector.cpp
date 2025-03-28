

#include <clang-c/Index.h>
#include <iostream>

#include "Core/Application/ApplicationGlobalState.h"
#include "Paths/Paths.h"
#include "Reflector/TypeReflector.h"


using namespace Lumina;

int main(int argc, char* argv[])
{
    FApplicationGlobalState State;

    const char* LuminaDirectory = std::getenv("LUMINA_DIR");
    FString LuminaEditor(LuminaDirectory);
    LuminaEditor += "/Lumina.sln";
    
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision( 2 );

    std::cout << "\n";
    std::cout << "===============================================" << "\n";
    std::cout << " * Lumina Reflector" << "\n";
    std::cout << "===============================================" << "\n" << "\n";

    if (!std::filesystem::exists(LuminaEditor.c_str()))
    {
        std::cerr << "Failed to find Lumina.sln";
    }

    Reflection::FTypeReflector TypeReflector(LuminaEditor);
    if (TypeReflector.ParseSolution())
    {
        
    }

    return 0;
}
