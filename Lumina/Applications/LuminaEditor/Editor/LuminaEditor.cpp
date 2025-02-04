#include "LuminaEditor.h"

#include "ImGui/ImGuiRenderer.h"
#include "Project/Project.h"
#include "Panels/ViewportLayer.h"
#include "EntryPoint.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Panels/ApplicationStats.h"
#include "Panels/AssetDebugWindow.h"
#include "Panels/ConsoleWindow.h"
#include "Panels/EditorSettingsPanel.h"
#include "Panels/EntityPropertyPanel.h"
#include "Panels/PerformanceTrackerPanel.h"
#include "Panels/ToolbarWindow.h"
#include "Panels/ContentBrowser/ContentBrowserWindow.h"
#include "Panels/Project/NewProjectPanel.h"
#include "Panels/Project/OpenProjectPanel.h"
#include "Panels/Project/PanelManager.h"
#include "Panels/Scene/SceneOutliner.h"
#include "Panels/Scene/SceneSettings.h"
#include "Scene/Scene.h"
#include "Settings/EditorSettings.h"

namespace Lumina
{
    FApplication* CreateApplication(int argc, char** argv)
    {
        return new LuminaEditor();
    }
    
    LuminaEditor::LuminaEditor()
        : FApplication("Lumina Editor", 1 << 0)
    {
    }
    
    bool LuminaEditor::Initialize()
    {
        //Update LUNINA_DIR (engine directory) every-time the editor is ran.
        //Paths::SetEnvVariable("LUMINA_DIR", std::filesystem::current_path().parent_path().string());
        LOG_DEBUG("LUMA_DIR Path: {0}", std::getenv("LUMINA_DIR"));
        LOG_DEBUG("Current Working Path: {0}",  std::filesystem::current_path().string());
        LOG_DEBUG("Current Engine Install Path: {0}", Paths::GetEngineInstallDirectory().string());
        LOG_DEBUG("Current Engine Directory: {0}", Paths::GetEngineDirectory().string());

        
        FEditorSettings::Get()->Deserialize();
        Project::Load(FEditorSettings::Get()->GetStartupProject());
        
        
        EditorCamera = FCamera::Create();

        
        return true;
    }

    bool LuminaEditor::ApplicationLoop()
    {
    }

    void LuminaEditor::CreateProject()
    {
        
    }

    void LuminaEditor::OpenProject()
    {
        
    }

    void LuminaEditor::CreateImGuiPanels()
    {
        //PanelManager::Get()->RegisterPanel<ViewportLayer>(FApplication::GetActiveScene());
       // PanelManager::Get()->RegisterPanel<SceneOutliner>(FApplication::GetActiveScene());
        PanelManager::Get()->RegisterPanel<ContentBrowserWindow>();
        PanelManager::Get()->RegisterPanel<ConsoleWindow>();
        PanelManager::Get()->RegisterPanel<ToolbarWindow>();
        PanelManager::Get()->RegisterPanel<AssetDebugWindow>();
        PanelManager::Get()->RegisterPanel<ApplicationStats>();
        PanelManager::Get()->RegisterPanel<NewProjectPanel>();
        PanelManager::Get()->RegisterPanel<OpenProjectPanel>();
        PanelManager::Get()->RegisterPanel<SceneSettings>();
        PanelManager::Get()->RegisterPanel<FEditorSettingsPanel>();
        PanelManager::Get()->RegisterPanel<FPerformanceTrackerPanel>();
        PanelManager::Get()->RegisterPanel<FEntityPropertyPanel>();
    }

    void LuminaEditor::RenderImGui(double DeltaTime)
    {
        ImGuiContext* currentContext = ImGui::GetCurrentContext();
        ImGuiViewport* mainViewport = ImGui::GetMainViewport();

        if (currentContext && mainViewport)
        {
            ImGui::DockSpaceOverViewport(mainViewport);
        }

        TVector<TRefPtr<EditorImGuiWindow>> Panels;
        PanelManager::Get()->GetPanels(Panels);

        for (auto& Panel : Panels)
        {
            if (Panel->IsVisible())
            {
                Panel->OnUpdate(DeltaTime);
            }
        }
    }

    void LuminaEditor::Shutdown()
    {
        
    }

    void LuminaEditor::OnEvent(FEvent& Event)
    {
        FApplication::OnEvent(Event);
    }
}
