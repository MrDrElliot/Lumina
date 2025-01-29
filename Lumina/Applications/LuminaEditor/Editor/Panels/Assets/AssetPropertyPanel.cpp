#include "AssetPropertyPanel.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include <imgui.h>

namespace Lumina
{
    bool FAssetPropertyPanel::Render(EAssetType Type, FAssetMetadata& Selected)
    {
        AssetRegistry* Registry = AssetRegistry::Get();
        TArray<FAssetMetadata> Assets;

        // Fetch all assets of the specified type
        Registry->GetAllAssetsOfType(Type, Assets);

            ImGui::Text("Select an Asset:");
            ImGui::Separator();

            // Make the list scrollable
            ImGui::BeginChild("AssetList", ImVec2(300, 400), true);

            for (FAssetMetadata& Meta : Assets)
            {
                // Begin a new item row
                ImGui::PushID(Meta.Name.c_str()); // Make sure each row has a unique ID

                ImGui::Bullet();
                ImGui::SameLine();
                
                // Display the asset name next to the image
                ImGui::SameLine();
                if (ImGui::Selectable(Meta.Name.c_str(), false))
                {
                    Selected = Meta;  // Update selected asset
                    ImGui::PopID(); // End the item row
                    ImGui::CloseCurrentPopup(); // Close the popup
                    ImGui::EndChild();
                    return true;
                }

                ImGui::PopID(); // End the item row
            }

            ImGui::EndChild();
            ImGui::Separator();

            if (ImGui::Button("Clear"))
            {
                Selected = {};
                ImGui::CloseCurrentPopup();
                return true;
            }
            
            ImGui::SameLine();
        
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }

        return false;
    }
}
