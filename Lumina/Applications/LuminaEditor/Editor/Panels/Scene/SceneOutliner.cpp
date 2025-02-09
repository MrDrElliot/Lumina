#include "SceneOutliner.h"

#include "imgui.h"
#include "Scene/Entity/Entity.h"
#include "EntityNode.h"

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
        if (mScene.expired())
        {
            return;
        }
        
        ImGui::Begin("Scene Outliner", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        
        // Custom light red color for the button
        ImVec4 buttonColor = ImVec4(0.8f, 0.3f, 0.3f, 0.8f);       // Normal state
        ImVec4 buttonHovered = ImVec4(0.9f, 0.4f, 0.4f, 0.9f);     // Hovered state
        ImVec4 buttonActive = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);      // Active state

        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActive);

        // Align the button to the right
        float buttonWidth = 80.0f; // Small button
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonWidth - 10.0f);

        // Add the button with a custom label
        if (ImGui::Button("+ Add", ImVec2(buttonWidth, 25)))
        {
            mScene.lock()->CreateEntity(FTransform(), "NewEntity");
        }

        // Restore original style colors
        ImGui::PopStyleColor(3);
        
        ImGui::SeparatorText("Entities");
        
        // Loop through entities and render them
        for (auto& Ent : mScene.lock()->GetEntityRegistry().view<FNameComponent>())
        {
            if (mScene.expired())
            {
                break;
            }
            
            Entity ent(Ent, mScene.lock());
            FEntityNode::Render(std::move(ent));
        }
        
        ImGui::End();
    }


    void SceneOutliner::OnEvent(FEvent& InEvent)
    {
    }
}
