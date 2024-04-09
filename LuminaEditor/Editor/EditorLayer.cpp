#include "EditorLayer.h"


#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneViewPanel.h"

namespace Lumina
{
    void FEditorLayer::OnAttach()
    {
        AddEditorLayer(new FSceneViewPanel);
        AddEditorLayer(new FContentBrowserPanel);
        
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
        for (auto Panel : EditorPanels)
        {
            Panel->OnImGui();
        }
    }

    void FEditorLayer::OnEvent()
    {
        FLayer::OnEvent();
    }

    void FEditorLayer::AddEditorLayer(FEditorPanel* NewPanel)
    {
        EditorPanels.emplace_back(NewPanel);
        NewPanel->OnAdded();
    }
}
