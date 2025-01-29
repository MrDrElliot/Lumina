#include "EntityNode.h"

#include "Scene/Components/NameComponent.h"
#include <imgui.h>
#include <Scene/Components/MeshComponent.h>

#include "Panels/SelectionManager.h"
#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/GuidComponent.h"
#include "Scene/Components/LightComponent.h"

namespace Lumina
{
    void EntityNode::Render(Entity& InEntity)
    {
        // Retrieve the NameComponent to get the entity's name
        auto& NameComponent = InEntity.GetComponent<FNameComponent>();
        const char* entityName = NameComponent.GetName().length() > 0 ? NameComponent.GetName().c_str() : "Unnamed Entity";

        // Start a tree node with a small rectangle style
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;

        // Determine selection state
        bool isSelected = FSelectionManager::Get()->IsSelected(ESelectionContext::SceneOutliner, InEntity.GetComponent<FGUIDComponent>().GetGUID());
        if (isSelected)
        {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }
        
        bool isNodeOpen = ImGui::TreeNodeEx(entityName, nodeFlags);

        // Right-click context menu for actions (Add Component, Delete)
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
            {
                InEntity.Destroy();
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }

        // Handle entity selection toggle
        if (ImGui::IsItemClicked())
        {
            if (isSelected)
            {
                FSelectionManager::Get()->RemoveSelection(ESelectionContext::SceneOutliner, InEntity.GetComponent<FGUIDComponent>().GetGUID());
            }
            else
            {
                FSelectionManager::Get()->AddSelection(ESelectionContext::SceneOutliner, InEntity.GetComponent<FGUIDComponent>().GetGUID());
            }
        }

        // If the tree node is expanded, show the entity's components
        if (isNodeOpen)
        {
            // Define background colors for components and groups
            ImVec4 componentBackgroundColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray background for components
            ImVec4 selectedBackgroundColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Lighter gray when selected
            ImVec4 groupBackgroundColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // Darker background for groups

            // Start a group with the specified background color for components
            ImGui::PushStyleColor(ImGuiCol_ChildBg, groupBackgroundColor);
            
            // Render components only if they exist
            if (InEntity.HasComponent<FTransformComponent>())
            {
                ImGui::Spacing();                
                ImGui::Text("Transform Component");
            }
            
            if (InEntity.HasComponent<FMeshComponent>())
            {
                ImGui::Spacing();
                ImGui::Text("Mesh Component");
            }
            
            if (InEntity.HasComponent<FLightComponent>())
            {
                ImGui::Spacing();
                ImGui::Text("Light Component");
            }
            
            if (InEntity.HasComponent<FCameraComponent>())
            {
                ImGui::Spacing();
                ImGui::Text("Camera Component");
            }

            // Pop the group style color after rendering components
            ImGui::PopStyleColor();

            // Render more components as needed

            ImGui::TreePop();
        }
    }
}

