#include "AssetDebugWindow.h"

#include "imgui.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetRegistry/AssetRegistry.h"

namespace Lumina
{
    void AssetDebugWindow::OnAttach()
    {
    }

    void AssetDebugWindow::OnDetach()
    {
    }

    void AssetDebugWindow::OnUpdate(double DeltaTime)
    {
        ImGui::Begin("Asset Debug");

        // Retrieve the list of alive assets from the Asset Manager
        TVector<TSharedPtr<LAsset>> AliveAssets;
        AssetManager::Get()->GetAliveAssets(AliveAssets);

        ImGui::PushStyleColor(0, {0.65f, 0.15f, 0.15f, 1.0});
        ImGui::SeparatorText("Asset Manager");
        ImGui::PopStyleColor();
        for (auto& alive : AliveAssets)
        {
            // If asset is valid, display asset info
            if (alive)
            {
                ImGui::Text("Asset Name: %s", alive->GetAssetMetadata().Name.c_str());
                ImGui::Text("Asset GUID: %llu", alive->GetAssetMetadata().Guid.Get());
                ImGui::Text("Asset Type: %s", AssetTypeToString(alive->GetAssetType()).c_str());
            
                ImGui::Separator(); // Separate the assets visually
            }
        }

        ImGui::PushStyleColor(0, {0.65, 0.15, 0.15, 1.0});
        ImGui::SeparatorText("Asset Registry");
        ImGui::PopStyleColor();
    
        TVector<FAssetMetadata> RegisteredAssets;
        AssetRegistry::Get()->GetAllRegisteredAssets(RegisteredAssets);
        for (auto& alive : RegisteredAssets)
        {
            ImGui::Text("Asset Name: %s", alive.Name.c_str());
            ImGui::Text("Asset GUID: %llu", alive.Guid.Get());
            ImGui::Text("Asset Type: %s", AssetTypeToString(alive.AssetType).c_str());
        
            ImGui::Separator(); // Separate the assets visually
        }

        ImGui::End();
    }


    void AssetDebugWindow::OnEvent(FEvent& InEvent)
    {
    }
}