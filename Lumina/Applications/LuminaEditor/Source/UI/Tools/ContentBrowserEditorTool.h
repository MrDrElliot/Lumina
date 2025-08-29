#pragma once
#include "EditorTool.h"
#include "Paths/Paths.h"
#include "Renderer/RHIFwd.h"
#include "Tools/UI/ImGui/imfilebrowser.h"
#include "Tools/UI/ImGui/Widgets/TileViewWidget.h"
#include "Tools/UI/ImGui/Widgets/TreeListView.h"

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
                //@TODO this is pretty gross.
                FString IconString(LE_ICON_FOLDER);
                FString NameString(GetName().c_str());
                FString CombinedString(IconString + " " + NameString);

                return FInlineString(CombinedString.c_str());
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
            
            FContentBrowserTileViewItem(FTileViewItem* InParent, const FString& InPath, bool bIsDirectory)
                : FTileViewItem(InParent)
                , bDirectory(bIsDirectory)
                , Path(InPath)
                , VirtualPath(Paths::ConvertToVirtualPath(InPath))
            {}

            constexpr static const char* DragDropID = "ContentBrowserItem";
            
            void SetDragDropPayloadData() const override
            {
                uintptr_t IntPtr = (uintptr_t)this;
                ImGui::SetDragDropPayload(DragDropID, &IntPtr, sizeof(uintptr_t));
            }

            const char* GetTooltipText() const override { return "Test"; }
            bool HasContextMenu() override { return true; }

            FName GetName() const override
            {
                return Paths::FileName(Path, true);
            }

            const FString& GetPath() const { return Path; }
            const FString& GetVirtualPath() const { return VirtualPath; }
            bool IsDirectory() const { return bDirectory; }
            
        private:

            uint32                  bDirectory:1=0;
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
