#include "SceneHierarchyPanel.h"

#include "imgui.h"


namespace Lumina
{
    FSceneHierarchyPanel::FSceneHierarchyPanel()
    {
        Name = "Scene Hieracrchy";

    }

    FSceneHierarchyPanel::~FSceneHierarchyPanel()
    {
    }

    void FSceneHierarchyPanel::OnImGui()
    {
        ImGui::Begin(Name.c_str());
        
        ImGui::End();
    }

    void FSceneHierarchyPanel::OnAdded()
    {
    }

    void FSceneHierarchyPanel::OnRemoved()
    {
    }

    void FSceneHierarchyPanel::OnRender()
    {
    }

    void FSceneHierarchyPanel::OnNewScene()
    {
    }
}
