#pragma once
#include "ContentBrowserWindow.h"
#include "ImGuiAssetImporter.h"
#include "Assets/AssetRegistry/AssetRegistry.h"

namespace Lumina
{
    class ImGui_MeshImporter : public ImGuiAssetImporter
    {
    public:

        void Render(ContentBrowserWindow* ContentBrowser) override
        {
            ImGui::Text("Mesh Import Options:");
            ImGui::Separator();
            ImGui::InputTextWithHint("Name", "Enter asset name",Name, sizeof(Name));

            // Separator before Import/Cancel buttons
            ImGui::Separator();
    
            // Import button
            if (ImGui::Button("Import", ImVec2(120, 0)))
            {
                AssetRegistry::ImportAsset(Name, nullptr, ContentBrowser->SelectedFile, ContentBrowser->SelectedDirectory);
                ImGui::CloseCurrentPopup();
                ContentBrowser->SelectedFile.clear();
            }
    
            // Cancel button
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup(); // Close the popup when cancel is pressed
                ContentBrowser->SelectedFile.clear();
            }
        }
    
    };
}
