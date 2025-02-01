#pragma once
#include "imgui.h"
#include "ImGuiDrawUtils.h"
#include "Panels/Assets/AssetPropertyPanel.h"
#include "Scene/Components/LightComponent.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Components/NameComponent.h"
#include "Scene/Components/TransformComponent.h"
#include "ThirdParty/glm/glm/gtc/type_ptr.inl"


namespace Lumina::ImGuiWidgets
{
    template<typename T>
    void DrawEntityComponent(T& Value, ImVec2 Padding = ImVec2(2.5f, 0.0f));
    
    template <>
    inline void DrawEntityComponent<FNameComponent>(FNameComponent& Component, ImVec2 Padding)
    {
        ImGui::Dummy(Padding);
        if (ImGui::CollapsingHeader("Name Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, Component.GetName().c_str());

            if (ImGui::InputText("##EntityName", buffer, sizeof(buffer)))
            {
                Component.GetName() = FString(buffer);
            }
        }
    }

    template <>
    inline void DrawEntityComponent<FTransformComponent>(FTransformComponent& Component, ImVec2 Padding)
    {
        ImGui::Dummy(Padding);
        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGuiUtils::DrawVec3Control("Location", Component.Transform.Location, 0.0f, 90.0f);

            glm::vec3 eulerAngles = glm::eulerAngles(Component.Transform.Rotation);
            ImGuiUtils::DrawVec3Control("Rotation", eulerAngles, 0.0f, 90.0f);
            Component.SetRotationFromEuler(eulerAngles);

            ImGuiUtils::DrawVec3Control("Scale", Component.Transform.Scale, 1.0f, 90.0f);
        }
    }

    template <>
    inline void DrawEntityComponent<FMeshComponent>(FMeshComponent& Component, ImVec2 Padding)
    {
        ImGui::Dummy(Padding);
        if (ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Spacing();
        
            ImGui::Columns(2);
                    
            const char* MaterialAssetName = Component.Material.IsValid() ? Component.Material->GetAssetMetadata().Name.c_str() : "No Material";
            ImGui::Text("Material: ");
        
            if (ImGui::Button(MaterialAssetName))
            {
                ImGui::OpenPopup("MaterialProperty_Popup");
            }
        
            ImGui::NextColumn();
        
            const char* MeshAssetName = Component.StaticMesh.IsValid() ? Component.StaticMesh->GetAssetMetadata().Name.c_str() : "No Mesh"; 
            ImGui::Text("Static Mesh: ");
        
            if (ImGui::Button(MeshAssetName))
            {
                ImGui::OpenPopup("MeshProperty_Popup");
            }
        
            if (ImGui::BeginPopup("MaterialProperty_Popup"))
            {
                FAssetHeader Metadata = {};
                if (FAssetPropertyPanel::Render(EAssetType::Material, Metadata))
                {
                    Component.Material = Metadata;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }
            ImGui::EndColumns();
        
            ImGui::Spacing();
        
            if (ImGui::BeginPopup("MeshProperty_Popup"))
            {
                FAssetHeader Metadata = {};
                if (FAssetPropertyPanel::Render(EAssetType::StaticMesh, Metadata))
                {
                    Component.StaticMesh = Metadata;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }

    
    template <>
    inline void DrawEntityComponent<FLightComponent>(FLightComponent& Component, ImVec2 Padding)
    {
        ImGui::Dummy(Padding);
        if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Light Properties");
            ImGui::Separator();
        
            // Color picker for LightColor (RGB)
            ImGui::Text("Light Color:");
            ImGui::ColorEdit3("##LightColor", glm::value_ptr(Component.LightColor));
        
            // Slider for intensity (stored in alpha)
            ImGui::Text("Intensity:");
            ImGui::SliderFloat("##LightIntensity", &Component.LightColor.a, 0.0f, 10.0f, "%.2f");
        
            ImGui::Spacing();
        }
    }
}
