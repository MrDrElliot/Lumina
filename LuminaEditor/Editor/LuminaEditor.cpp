#include "LuminaEditor.h"

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

namespace Lumina
{
    FApplication* CreateApplication(int argc, char** argv)
    {
        FApplicationSpecs AppSpecs;
        AppSpecs.Name = "Lumina Editor";
        AppSpecs.WindowWidth = 1600;
        AppSpecs.WindowHeight = 900;
        return new LuminaEditor(AppSpecs);
    }

    LuminaEditor::LuminaEditor(const FApplicationSpecs& AppSpecs): FApplication(AppSpecs)
    {
        
    }

    LuminaEditor::~LuminaEditor()
    {
    }
}
