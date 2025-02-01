#include "EntityPropertyPanel.h"

#include "ImGuiWidgets/ImGuiComponentWidgets.h"
#include "SelectionManager.h"
#include "Core/Application/Application.h"
#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Entity/Entity.h"

#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/LightComponent.h"


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
        TVector<FGuid> Selections;
        FSelectionManager::Get()->GetSelections(ESelectionContext::SceneOutliner, Selections);

        // Get the active scene
        TSharedPtr<AScene> Scene = FApplication::Get().GetActiveScene();

        // Begin ImGui window for Entity Properties
        ImGui::Begin("Entity Properties");
        
        for (FGuid& Selection : Selections)
        {
            bool bFound = false;
            Entity Ent = Scene->GetEntityByGUID(Selection, &bFound);

            if (bFound)
            {
                const char* EntityName = Ent.GetName().c_str();

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
                    ImGui::TextWrapped("Select a component to add to the selected entity.");
                    ImGui::Separator();

                    ImGui::Spacing();

                    ImGui::Text("Available Components:");
                    ImGui::Spacing();

                    ImGui::BeginChild("ComponentList", ImVec2(400.0f, 200.0f), true);

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

                if (Ent.HasComponent<FNameComponent>())
                {
                    ImGuiWidgets::DrawEntityComponent(Ent.GetComponent<FNameComponent>());
                }
                
                if (Ent.HasComponent<FTransformComponent>())
                {
                    ImGuiWidgets::DrawEntityComponent(Ent.GetComponent<FTransformComponent>());
                }
                
                if (Ent.HasComponent<FMeshComponent>())
                {
                    ImGuiWidgets::DrawEntityComponent(Ent.GetComponent<FMeshComponent>());
                }

                if (Ent.HasComponent<FLightComponent>())
                {
                    ImGuiWidgets::DrawEntityComponent(Ent.GetComponent<FLightComponent>());
                    
                }
            }
        }
        ImGui::End();
    }

     

    void FEntityPropertyPanel::OnEvent(FEvent& InEvent)
    {
    }
}
