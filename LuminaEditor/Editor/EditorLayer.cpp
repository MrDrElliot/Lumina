
#include "EditorLayer.h"

#include "Core/Application.h"
#include "Panels/ApplicationStats.h"
#include "Panels/AssetDebugWindow.h"
#include "Panels/ConsoleWindow.h"
#include "Panels/EditorSettingsPanel.h"
#include "Panels/EntityPropertyPanel.h"
#include "Panels/PerformanceTrackerPanel.h"
#include "Panels/ToolbarWindow.h"
#include "Panels/ViewportLayer.h"
#include "Panels/ContentBrowser/ContentBrowserWindow.h"
#include "Panels/Project/NewProjectPanel.h"
#include "Panels/Project/OpenProjectPanel.h"
#include "Panels/Project/PanelManager.h"
#include "Panels/Scene/SceneOutliner.h"
#include "Panels/Scene/SceneSettings.h"


namespace Lumina
{
    void FEditorLayer::OnAttach()
    {
        PanelManager::Get()->RegisterPanel<ViewportLayer>(FApplication::GetActiveScene());
        PanelManager::Get()->RegisterPanel<SceneOutliner>(FApplication::GetActiveScene());
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

    void FEditorLayer::OnDetach()
    {
        FLayer::OnDetach();
    }

    void FEditorLayer::OnUpdate(double DeltaTime)
    {
    
    }

    void FEditorLayer::OnEvent(FEvent& InEvent)
    {
        FLayer::OnEvent(InEvent);
    }

    void FEditorLayer::ImGuiRender(double DeltaTime)
    {
        ImGuiContext* currentContext = ImGui::GetCurrentContext();
        ImGuiViewport* mainViewport = ImGui::GetMainViewport();

        if (currentContext && mainViewport)
        {
            ImGui::DockSpaceOverViewport(mainViewport);
        }

        TFastVector<TRefPtr<EditorImGuiWindow>> Panels;
        PanelManager::Get()->GetPanels(Panels);

        for (auto& Panel : Panels)
        {
            if (Panel->IsVisible())
            {
                Panel->OnUpdate(DeltaTime);
            }
        }
    }


    TRefPtr<EditorImGuiWindow> FEditorLayer::GetEditorWindowByName(const LString& Name)
    {
        TFastVector<TRefPtr<EditorImGuiWindow>> Windows;
        PanelManager::Get()->GetPanels(Windows);
        for (auto& Window : Windows)
        {
            if(Window->GetName() == Name)
            {
                return Window;
            }
        }

        return TRefPtr<EditorImGuiWindow>();
    }
}

