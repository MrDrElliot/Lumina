#pragma once
#include "EditorTool.h"
#include "Assets/AssetRegistry/AssetData.h"
#include "Core/Object/ObjectRedirector.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Renderer/RHIFwd.h"
#include "Tools/UI/ImGui/imfilebrowser.h"
#include "Tools/UI/ImGui/Widgets/TileViewWidget.h"
#include "Tools/UI/ImGui/Widgets/TreeListView.h"

namespace Lumina
{
    class CObjectRedirector;
}

namespace Lumina
{
    struct FAssetData;
}

namespace Lumina
{
    class FContentBrowserEditorTool : public FEditorTool
    {
    public:

        class FContentBrowserListViewItem : public FTreeListViewItem
        {
        public:

            FContentBrowserListViewItem(FTreeListViewItem* InParent, const FString& InPath, const FString& InDisplayName)
                : FTreeListViewItem(InParent)
                , DisplayName(InDisplayName)
                , Path(InPath)
            {}
            
            virtual ~FContentBrowserListViewItem() override { }

            const char* GetTooltipText() const override { return Path.c_str(); }
            bool HasContextMenu() override { return true; }
            
            
            FInlineString GetDisplayName() const override
            {
                return FInlineString()
                .append(LE_ICON_FOLDER)
                .append(" ")
                .append(GetName().c_str());
            }

            FName GetName() const override
            {
                return DisplayName.c_str();
            }

            uint64 GetHash() const override { return Hash::GetHash64(Path); }

            const FString& GetPath() const { return Path; }
            
        private:

            FString DisplayName;
            FString Path;
        };

        class FContentBrowserTileViewItem : public FTileViewItem
        {
        public:
            
            FContentBrowserTileViewItem(FTileViewItem* InParent, const FString& InPath, FAssetData* InAssetData)
                : FTileViewItem(InParent)
                , Path(InPath)
                , VirtualPath(Paths::ConvertToVirtualPath(InPath))
            {
                if (InAssetData)
                {
                    AssetData = *InAssetData;
                }
            }

            constexpr static const char* DragDropID = "ContentBrowserItem";
            
            void SetDragDropPayloadData() const override
            {
                uintptr_t IntPtr = (uintptr_t)this;
                ImGui::SetDragDropPayload(DragDropID, &IntPtr, sizeof(uintptr_t));
            }

            void DrawTooltip() const override
            {
                bool bIsAsset = !AssetData.AssetClass.IsNone();
                
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 200, 50, 255));
                ImGui::TextUnformatted(Paths::FileName(VirtualPath).c_str());
                ImGui::PopStyleColor();

                if (bIsAsset)
                {
                    ImGui::Text(LE_ICON_FILE " %s", AssetData.AssetClass.c_str());
                }
                else
                {
                    ImGui::TextUnformatted(LE_ICON_FOLDER " Directory");
                }
                
                ImGui::Separator();

                ImGui::Text(LE_ICON_FOLDER " %s", VirtualPath.c_str());

                if (bIsAsset)
                {
                    FString FullPath = Path + ".lasset";
                    
                    if (std::filesystem::exists(FullPath.c_str()))
                    {
                        uint64 Size = std::filesystem::file_size(FullPath.c_str());
                        ImGui::Text(LE_ICON_FILE_CODE " Size: %llu", Size);
                    }
                }
            }
            
            bool HasContextMenu() override { return true; }

            FName GetName() const override
            {
                FInlineString NameString;
                if (!AssetData.PackageName.IsNone())
                {
                    if (CPackage* Package = FindObject<CPackage>(nullptr, AssetData.PackageName))
                    {
                        if (Package->IsDirty())
                        {
                            NameString.append(Paths::FileName(Path, true).c_str()).append(" " LE_ICON_ARCHIVE_ALERT);
                            return NameString;
                        }
                    }
                }
                
                NameString.append(Paths::FileName(Path, true).c_str());
                return NameString;
            }

            FString GetFileName() const { return Paths::FileName(Path, true); }

            void SetPath(FStringView NewPath) { Path = NewPath; Paths::ConvertToVirtualPath(Path); }
            const FString& GetPath() const { return Path; }
            const FString& GetVirtualPath() const { return VirtualPath; }
            bool IsDirectory() const { return AssetData.AssetClass.IsNone(); }
            const FAssetData& GetAssetData() const { return AssetData; }
            
        private:

            FAssetData              AssetData;
            FString                 Path;
            FString                 VirtualPath;
        };

        LUMINA_SINGLETON_EDITOR_TOOL(FContentBrowserEditorTool)

        FContentBrowserEditorTool(IEditorToolContext* Context)
            : FEditorTool(Context, "Content Browser", nullptr)
            , OutlinerContext()
            , ContentBrowserTileView()
            , ContentBrowserTileViewContext()
        {
        }

        void RefreshContentBrowser();
        bool IsSingleWindowTool() const override { return true; }
        const char* GetTitlebarIcon() const override { return LE_ICON_FORMAT_LIST_BULLETED_TYPE; }
        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override { }

        void Update(const FUpdateContext& UpdateContext) override;

        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;


        void HandleContentBrowserDragDrop(FContentBrowserTileViewItem* Drop, FContentBrowserTileViewItem* Payload);
        
    private:

        bool HandleRenameEvent(const FString& OldPath, FString NewPath);
        
        void DrawDirectoryBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size);
        void DrawContentBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size);

        FTreeListView               OutlinerListView;
        FTreeListViewContext        OutlinerContext;

        FTileViewWidget             ContentBrowserTileView;
        FTileViewContext            ContentBrowserTileViewContext;

        FString                     SelectedPath;
        ImGui::FileBrowser          FileBrowser;
    };
}
