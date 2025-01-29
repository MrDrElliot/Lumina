#include "EntityPropertyPanel.h"

#include "ImGuiDrawUtils.h"
#include "SelectionManager.h"
#include "Core/Application/Application.h"
#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Entity/Entity.h"
#include "Assets/AssetPropertyPanel.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"
#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/LightComponent.h"
#include "ThirdParty/glm/glm/gtc/type_ptr.inl"


namespace Lumina
{
    void FEntityPropertyPanel::OnAttach()
    {
    }

    void FEntityPropertyPanel::OnDetach()
    {
    }

    void FEntityPropertyPanel::OnUpdate(double DeltaTime)
    {
        // Retrieve the list of selected entities' GUIDs from the selection manager
        TArray<FGuid> Selections;
        FSelectionManager::Get()->GetSelections(ESelectionContext::SceneOutliner, Selections);

        // Get the active scene
        TSharedPtr<LScene> Scene = FApplication::Get().GetActiveScene();

        // Begin ImGui window for Entity Properties
        ImGui::Begin("Entity Properties");

        // Iterate through each selected entity
        for (FGuid& Selection : Selections)
        {
            // Look up the entity by GUID
            bool bFound = false;
            Entity Ent = Scene->GetEntityByGUID(Selection, &bFound);

            if (bFound)
            {
                // Display basic entity information (e.g., name or type)
                const char* EntityName = Ent.GetName().CStr();

                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
                ImGui::Text("Entity: %s", EntityName);
                ImGui::PopFont();

                // Add Component button
                if (ImGui::Button("Add Component"))
                {
                    ImGui::OpenPopup("New Component");
                }

                bool bOpen = true;
                if (ImGui::BeginPopupModal("New Component", &bOpen, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    // Modal Header
                    ImGui::TextWrapped("Select a component to add to the selected entity."); // Add some wrapping for long text
                    ImGui::Separator(); // A line separator for clean organization

                    ImGui::Spacing();

                    // Modal Content: List of Components
                    ImGui::Text("Available Components:");
                    ImGui::Spacing();

                    // Start a child window to add scrollability for long lists of components
                    ImGui::BeginChild("ComponentList", ImVec2(400.0f, 200.0f), true);

                    // Improved Selectables with a clean box-like look
                    if (ImGui::Selectable("Mesh Component", false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        Ent.AddComponent<FMeshComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Selectable("Camera Component", false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        Ent.AddComponent<FCameraComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Selectable("Light Component", false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        Ent.AddComponent<FLightComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                    // End scrollable child window
                    ImGui::EndChild();

                    ImGui::EndPopup();
                }
                if(!bOpen)
                {
                    ImGui::CloseCurrentPopup();
                }



                ImGui::SeparatorText("Components");

                // Example: Display Transform Component properties
                if (Ent.HasComponent<FTransformComponent>())
                {
                    auto& TransformComponent = Ent.GetComponent<FTransformComponent>();

                    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::Spacing();

                        ImGuiUtils::DrawVec3Control("Location", TransformComponent.Transform.Location, 0.0f, 90.0f);

                        glm::vec3 eulerAngles = glm::eulerAngles(TransformComponent.Transform.Rotation);
                        ImGuiUtils::DrawVec3Control("Rotation", eulerAngles, 0.0f, 90.0f);
                        TransformComponent.SetRotationFromEuler(eulerAngles);

                        ImGuiUtils::DrawVec3Control("Scale", TransformComponent.Transform.Scale, 1.0f, 90.0f);

                    }
                }

                ImGui::Dummy(ImVec2(2.5f, 0.0f));

                if (Ent.HasComponent<FMeshComponent>())
                {
                    ImGui::Spacing();
    
                    auto& MeshComponent = Ent.GetComponent<FMeshComponent>();

                    if (ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::Spacing();

                        ImGui::Columns(2);
                        
                        const char* MaterialAssetName = MeshComponent.Material.IsValid() ? MeshComponent.Material->GetAssetMetadata().Name.c_str() : "No Material";
                        
                        ImGui::Text("Material: ");
        
                        if (ImGui::Button(MaterialAssetName))
                        {
                            ImGui::OpenPopup("MaterialProperty_Popup");
                        }

                        ImGui::NextColumn();

                        const char* MeshAssetName = MeshComponent.StaticMesh.IsValid() ? MeshComponent.StaticMesh->GetAssetMetadata().Name.c_str() : "No Mesh"; 
                        ImGui::Text("Static Mesh: ");
        
                        if (ImGui::Button(MeshAssetName))
                        {
                            ImGui::OpenPopup("MeshProperty_Popup");
                        }

                        // Material Popup
                        if (ImGui::BeginPopup("MaterialProperty_Popup"))
                        {
                            FAssetMetadata Metadata = {};
                            if (FAssetPropertyPanel::Render(EAssetType::Material, Metadata))
                            {
                                MeshComponent.Material = Metadata;
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }
                        ImGui::EndColumns();

                        ImGui::Spacing();

                        // Mesh Popup
                        if (ImGui::BeginPopup("MeshProperty_Popup"))
                        {
                            FAssetMetadata Metadata = {};
                            if (FAssetPropertyPanel::Render(EAssetType::StaticMesh, Metadata))
                            {
                                MeshComponent.StaticMesh = Metadata;
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }
                    }
                }

                if (Ent.HasComponent<FLightComponent>())
                {
                    ImGui::Spacing();
    
                    auto& LightComponent = Ent.GetComponent<FLightComponent>();

                    if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::Text("Light Properties");
                        ImGui::Separator();

                        // Color picker for LightColor (RGB)
                        ImGui::Text("Light Color:");
                        ImGui::ColorEdit3("##LightColor", glm::value_ptr(LightComponent.LightColor));

                        // Slider for intensity (stored in alpha)
                        ImGui::Text("Intensity:");
                        ImGui::SliderFloat("##LightIntensity", &LightComponent.LightColor.a, 0.0f, 10.0f, "%.2f");

                        ImGui::Spacing();
                    }
                }

            }
        }

        // End ImGui window
        ImGui::End();
    }

     

    void FEntityPropertyPanel::OnEvent(FEvent& InEvent)
    {
    }
}
