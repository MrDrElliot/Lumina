#include "LuminaEditor.h"

#include "ImGui/ImGuiRenderer.h"
#include "Project/Project.h"
#include "Panels/ViewportLayer.h"
#include "EntryPoint.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Scene/Scene.h"
#include "Settings/EditorSettings.h"

namespace Lumina
{
    FApplication* CreateApplication(int argc, char** argv)
    {
        FApplicationSpecs AppSpecs;
        AppSpecs.Name = "Lumina Editor";
        AppSpecs.bRenderImGui = true;
        
        return new LuminaEditor(AppSpecs);
    }
    
    LuminaEditor::LuminaEditor(const FApplicationSpecs& AppSpecs)
        : FApplication(AppSpecs)
    {
    }
    
    void LuminaEditor::OnInit()
    {
        GEditor = this;

        //Update LUNINA_DIR (engine directory) every-time the editor is ran.
        //Paths::SetEnvVariable("LUMINA_DIR", std::filesystem::current_path().parent_path().string());
        LOG_DEBUG("LUMA_DIR Path: {0}", std::getenv("LUMINA_DIR"));
        LOG_DEBUG("Current Working Path: {0}",  std::filesystem::current_path().string());
        LOG_DEBUG("Current Engine Install Path: {0}", Paths::GetEngineInstallDirectory().string());
        LOG_DEBUG("Current Engine Directory: {0}", Paths::GetEngineDirectory().string());

        FApplication::OnInit();
        
        FEditorSettings::Get()->Deserialize();
        Project::Load(FEditorSettings::Get()->GetStartupProject());
        AssetRegistry::Get()->StartAssetScan();
        
        
        EditorCamera = FCamera::Create();
        SetCurrentScene(AScene::Create(EditorCamera));
        
        EditorLayer = MakeRefPtr<FEditorLayer>("Editor Layer");
        PushLayer(EditorLayer);
        
    }

    void LuminaEditor::CreateProject()
    {
        
    }

    void LuminaEditor::OpenProject()
    {
        
    }

    void LuminaEditor::OnShutdown()
    {
        AssetRegistry::Get()->Shutdown();
        FApplication::OnShutdown();
    }

    void LuminaEditor::OnEvent(FEvent& Event)
    {
        FApplication::OnEvent(Event);
    }
}
