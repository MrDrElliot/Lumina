#include "LuminaEditor.h"

#include "ImGui/ImGuiRenderer.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "Panels/ConsoleWindow.h"
#include "Panels/ViewportLayer.h"
#include "EntryPoint.h"

namespace Lumina
{
    std::unique_ptr<FApplication> CreateApplication(int argc, char** argv)
    {
        FApplicationSpecs AppSpecs;
        AppSpecs.Name = "Lumina Editor";

        AppSpecs.WindowWidth = 1920/1.3;
        AppSpecs.WindowHeight = 1080/1.3;;
        
        return std::make_unique<LuminaEditor>(AppSpecs);
    }
    
    LuminaEditor::LuminaEditor(const FApplicationSpecs& AppSpecs)
    : FApplication(AppSpecs)
    {
    }

    LuminaEditor::~LuminaEditor()
    {
        
    }

    void LuminaEditor::OnInit()
    {
        GEditor = this;
        FApplication::OnInit();

        EditorLayer = MakeRefPtr<FEditorLayer>("Editor Layer");
        PushLayer(EditorLayer);

        TRefPtr<FEditorLayer> Layer = GetLayerByType<FEditorLayer>();
        
        Project::Load("../Sandbox/Sandbox.lproject");
    }

    void LuminaEditor::PostFrame()
    {
        FApplication::PostFrame();
    }

    void LuminaEditor::CreateProject()
    {
        
    }

    void LuminaEditor::OpenProject()
    {
        
    }

    void LuminaEditor::OnShutdown()
    {
        FApplication::OnShutdown();
    }

    void LuminaEditor::OnEvent(FEvent& Event)
    {
        FApplication::OnEvent(Event);
    }
}