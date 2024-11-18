#include "EntityNode.h"

#include "Scene/Components/NameComponent.h"
#include <imgui.h>
#include <Scene/Components/MeshComponent.h>

#include "Panels/SelectionManager.h"
#include "Scene/Components/UniqueComponent.h"

namespace Lumina
{
    void EntityNode::Render(Entity& InEntity)
    {
        // Retrieve the NameComponent to get the entity's name
        auto& NameComponent = InEntity.GetComponent<FNameComponent>();
        const char* entityName = NameComponent.GetName().Length() > 0 ? NameComponent.GetName().CStr() : "Unnamed Entity";

        // Start a tree node with a small rectangle style
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding;

        // Determine selection state
        bool isSelected = FSelectionManager::Get()->IsSelected(ESelectionContext::SceneOutliner, InEntity.GetComponent<FUniqueComponent>().GetGUID());
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
                FSelectionManager::Get()->RemoveSelection(ESelectionContext::SceneOutliner, InEntity.GetComponent<FUniqueComponent>().GetGUID());
            }
            else
            {
                FSelectionManager::Get()->AddSelection(ESelectionContext::SceneOutliner, InEntity.GetComponent<FUniqueComponent>().GetGUID());
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
                bool isComponentSelected = isSelected;  // Keep track of selection state for each component
                
                // Apply selection style if selected
                ImVec4 componentStyle = isComponentSelected ? selectedBackgroundColor : componentBackgroundColor;
                ImGui::PushStyleColor(ImGuiCol_Button, componentStyle);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, componentStyle);

                // Highlightable/selectable component block for Transform
                if (ImGui::Selectable("Transform Component", false, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    // Handle selection or double-click logic if needed
                }

                ImGui::PopStyleColor(2); // Pop the style changes for the next components
            }
            
            if (InEntity.HasComponent<FMeshComponent>())
            {
                ImGui::Spacing();
                bool isComponentSelected = isSelected;  // Keep track of selection state for each component
                
                // Apply selection style if selected
                ImVec4 componentStyle = isComponentSelected ? selectedBackgroundColor : componentBackgroundColor;
                ImGui::PushStyleColor(ImGuiCol_Button, componentStyle);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, componentStyle);

                // Highlightable/selectable component block for Mesh
                if (ImGui::Selectable("Mesh Component", false, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    // Handle selection or double-click logic if needed
                }

                ImGui::PopStyleColor(2); // Pop the style changes for the next components
            }

            // Pop the group style color after rendering components
            ImGui::PopStyleColor();

            // Render more components as needed

            ImGui::TreePop();
        }
    }
}

