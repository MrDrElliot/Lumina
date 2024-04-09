#include "LuminaEditor.h"

#include <filesystem>
#include <iostream>

#include "EditorLayer.h"
#include "Panels/SceneViewPanel.h"


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
        PushOverlay(new FEditorLayer);
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
