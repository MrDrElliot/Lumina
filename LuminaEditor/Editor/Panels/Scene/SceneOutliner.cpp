#include "SceneOutliner.h"

#include "imgui.h"
#include "Scene/Entity/Entity.h"

namespace Lumina
{
    void SceneOutliner::OnAttach()
    {
    }

    void SceneOutliner::OnDetach()
    {
    }

    void SceneOutliner::OnUpdate(double DeltaTime)
    {
        ImGui::Begin("Scene Outliner", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        
        // Entity Section
        ImGui::SeparatorText("Entities");
        
        ImGui::End();
    }


    void SceneOutliner::OnEvent(FEvent& InEvent)
    {
    }
}
