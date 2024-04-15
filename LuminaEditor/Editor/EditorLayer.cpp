#include "EditorLayer.h"


#include "Panels/ConsolePanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MainMenuPanel.h"
#include "Panels/PropertyDetailsPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Source/Runtime/ImGui/ImGuiFonts.h"

namespace Lumina
{
    void FEditorLayer::OnAttach()
    {
        AddEditorLayer(new FMainMenuPanel);
        AddEditorLayer(new FSceneViewPanel);
        AddEditorLayer(new FContentBrowserPanel);
        AddEditorLayer(new FSceneHierarchyPanel);
        AddEditorLayer(new FConsolePanel);
        AddEditorLayer(new FPropertyDetailsPanel);

    }

    void FEditorLayer::OnDetach()
    {
        FLayer::OnDetach();
    }

    void FEditorLayer::OnUpdate(float DeltaTime)
    {
        for (auto Panel : EditorPanels)
        {
            Panel->OnRender();
        }
    }

    void FEditorLayer::OnImGuiRender()
    {
        if(ImGui::GetCurrentContext() && ImGui::GetMainViewport())
        {
            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        }
        
        Font::PushFont("Roboto-Bold");
        
        for (auto Panel : EditorPanels)
        {
            Panel->OnImGui();
        }

        Font::PopFont();
    }

    void FEditorLayer::OnEvent(FEvent& InEvent)
    {
        FLayer::OnEvent(InEvent);
    }

    void FEditorLayer::AddEditorLayer(FEditorPanel* NewPanel)
    {
        EditorPanels.emplace_back(NewPanel);
        NewPanel->OnAdded();
    }
}
