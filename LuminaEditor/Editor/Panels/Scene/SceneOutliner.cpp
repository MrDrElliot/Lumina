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
        ImGui::Begin("Scene Outliner", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        // Define custom styling for the button
        ImVec4 originalButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 originalButtonHovered = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        ImVec4 originalButtonActive = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);

        // Custom light red color for the button
        ImVec4 buttonColor = ImVec4(0.8f, 0.3f, 0.3f, 0.8f);       // Normal state
        ImVec4 buttonHovered = ImVec4(0.9f, 0.4f, 0.4f, 0.9f);     // Hovered state
        ImVec4 buttonActive = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);      // Active state

        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActive);

        // Align the button to the right
        float buttonWidth = 80.0f; // Small button
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonWidth - 10.0f); // Align with 10px padding from the right

        // Add the button with a custom label
        if (ImGui::Button("+ Add", ImVec2(buttonWidth, 25)))
        {
            mScene->CreateEntity(FTransform(), "NewEntity");
        }

        // Restore original style colors
        ImGui::PopStyleColor(3);
        
        ImGui::SeparatorText("Entities");
        
        // Loop through entities and render them
        for (auto& Ent : mScene->GetEntityRegistry().view<FNameComponent>())
        {
            Entity ent(Ent, mScene);
            EntityNode::Render(ent);
        }
        

        ImGui::End();


    }


    void SceneOutliner::OnEvent(FEvent& InEvent)
    {
    }
}
