#include "EntityPropertyPanel.h"

#include "SelectionManager.h"
#include "Core/Application.h"
#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Entity/Entity.h"


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
    TFastVector<FGuid> Selections;
    FSelectionManager::Get()->GetSelections(ESelectionContext::SceneOutliner, Selections);

    // Get the active scene
    std::shared_ptr<LScene> Scene = FApplication::Get().GetActiveScene();

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
            ImGui::Text("Entity: %s", EntityName);
            ImGui::Separator(); // Separate entity name from other properties

            // Example: Display Transform Component properties
            if (Ent.HasComponent<FTransformComponent>())
            {
                auto& TransformComponent = Ent.GetComponent<FTransformComponent>();

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
