/*#pragma once

#include "imgui.h"
#include <Plugins/ImFileBrowser.h>
#include "ContentBrowserItem.h"
#include "Assets/AssetHandle.h"
#include "Renderer/Image.h"


namespace Lumina
{
    class ContentBrowserItem;
    class ATexture;
    
    class ContentBrowserWindow : public EditorImGuiWindow
    {
    public:

        friend class ImGuiAssetImporter;

        ContentBrowserWindow()
            :FolderTexture(), ImFolderTexture(nullptr), ImAssetTexture(nullptr)
        {
            Name = "Content Browser";
            bVisible = true;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;

        void RefreshItems(const std::filesystem::path& InPath = std::filesystem::path());
        void RenderContentItems();

    
        ImGui::FileBrowser fileDialog;
    
        TRefPtr<FImage> FolderTexture;
        ImTextureID ImFolderTexture;

        TRefPtr<FImage> AssetTexture;
        ImTextureID ImAssetTexture;
    
        std::string SelectedDirectory;
        std::filesystem::path SelectedFile;
        TVector<TRefPtr<ContentBrowserItem>> ContentItemEntries;
        std::map<EAssetType, TSharedPtr<ImGuiAssetImporter>> mImGuiImporterMap;
    };
}*/