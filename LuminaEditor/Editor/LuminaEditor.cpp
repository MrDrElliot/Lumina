#include "LuminaEditor.h"

#include <filesystem>
#include <iostream>

#include "EditorLayer.h"
#include "Panels/SceneViewPanel.h"


namespace fs = std::filesystem;

namespace Lumina
{
    std::unique_ptr<FApplication> CreateApplication(int argc, char** argv)
    {
        FApplicationSpecs AppSpecs;
        AppSpecs.Name = "Lumina Editor";
        AppSpecs.WindowWidth = 1600;
        AppSpecs.WindowHeight = 900;
        return std::make_unique<LuminaEditor>(AppSpecs);
    }

    LuminaEditor::LuminaEditor(const FApplicationSpecs& AppSpecs): FApplication(AppSpecs)
    {
    }

    LuminaEditor::~LuminaEditor()
    {
    }

    void LuminaEditor::OnInit()
    {
        FApplication::OnInit();

    }

    void LuminaEditor::RenderImGui()
    {
        FApplication::RenderImGui();
    }

}
