#include "LuminaEditor.h"

#include "ImGui/ImGuiRenderer.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "Panels/ConsoleWindow.h"
#include "Panels/ViewportLayer.h"
#include "EntryPoint.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Settings/EditorSettings.h"

namespace Lumina
{
    std::unique_ptr<FApplication> CreateApplication(int argc, char** argv)
    {
        FApplicationSpecs AppSpecs;
        AppSpecs.Name = "Lumina Editor";

        AppSpecs.WindowWidth = 1920*2;
        AppSpecs.WindowHeight = 1080*2;
        
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

        // Update LUNINA_DIR (engine directory) every-time the editor is ran.
        Paths::SetEnvVariable("LUMINA_DIR", std::filesystem::current_path().parent_path().string());
        
        FApplication::OnInit();

        FEditorSettings::Get()->Deserialize();
        
        EditorLayer = MakeRefPtr<FEditorLayer>("Editor Layer");
        PushLayer(EditorLayer);
        
        Project::Load(FEditorSettings::Get()->GetStartupProject());

        AssetRegistry::Get()->StartAssetScan();
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

        AssetRegistry::Get()->Shutdown();
    }

    void LuminaEditor::OnEvent(FEvent& Event)
    {
        FApplication::OnEvent(Event);
    }
}
