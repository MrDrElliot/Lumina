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
        TVector<TSharedPtr<IAsset>> TrackedAssets;
        FApplication::GetSubsystem<FAssetManager>()->GetTrackedAssets(TrackedAssets);

        ImGui::PushStyleColor(0, {0.65f, 0.15f, 0.15f, 1.0});
        ImGui::SeparatorText("Asset Manager");
        ImGui::PopStyleColor();
        for (auto& Asset : TrackedAssets)
        {
            // If asset is valid, display asset info
            if (Asset != nullptr)
            {
                ImGui::Text("Asset Name: %s", Asset->GetAssetMetadata().Name.c_str());
                ImGui::Text("Asset Ref Count: %i", Asset.use_count());
            
                ImGui::Separator();
            }
        }

        ImGui::PushStyleColor(0, {0.65, 0.15, 0.15, 1.0});
        ImGui::SeparatorText("Asset Registry");
        ImGui::PopStyleColor();
    
        TVector<FAssetHeader> RegisteredAssets;
        AssetRegistry::Get()->GetAllRegisteredAssets(RegisteredAssets);
        for (auto& alive : RegisteredAssets)
        {
            ImGui::Text("Asset Name: %s", alive.Name.c_str());
            ImGui::Separator();
        }

        ImGui::End();
    }


    void AssetDebugWindow::OnEvent(FEvent& InEvent)
    {
    }
}