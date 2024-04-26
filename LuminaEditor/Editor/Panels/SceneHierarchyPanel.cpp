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

    void FSceneHierarchyPanel::OnAdded()
    {
    }

    void FSceneHierarchyPanel::OnRemoved()
    {
    }

    void FSceneHierarchyPanel::OnRender(double DeltaTime)
    {
        ImGui::Begin(Name.c_str());
        
        ImGui::End();
    }

    void FSceneHierarchyPanel::OnNewScene()
    {
    }
}
