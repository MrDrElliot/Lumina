#pragma once

#include "imgui.h"
#include <Plugins/ImFileBrowser.h>
#include "ContentBrowserItem.h"
#include "ContentBrowserWindow.h"
#include "Assets/AssetHandle.h"
#include "Renderer/Image.h"
#include "Panels/ImGuiWindow.h"



namespace Lumina
{
    class ContentBrowserItem;
    class LTexture;
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

        void OnNewDirectorySelected(const std::filesystem::path& InPath);
        void RenderContentItems();

    
        ImGui::FileBrowser fileDialog;
    
        TRefPtr<FImage> FolderTexture;
        ImTextureID ImFolderTexture;

        TRefPtr<FImage> AssetTexture;
        ImTextureID ImAssetTexture;
    
        std::string SelectedDirectory;
        std::filesystem::path SelectedFile;
        TFastVector<TRefPtr<ContentBrowserItem>> ContentItemEntries;
        std::map<EAssetType, std::shared_ptr<ImGuiAssetImporter>> mImGuiImporterMap;
    };
}