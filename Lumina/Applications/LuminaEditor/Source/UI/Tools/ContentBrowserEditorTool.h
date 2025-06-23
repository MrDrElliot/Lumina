#pragma once
#include "EditorTool.h"
#include "Paths/Paths.h"
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

            FContentBrowserListViewItem(FTreeListViewItem* InParent, std::filesystem::path InPath)
                : FTreeListViewItem(InParent)
                , Path(std::move(InPath))
            {}

            virtual ~FContentBrowserListViewItem() override { }

            const char* GetTooltipText() const override { return Path.filename().string().c_str(); }
            bool HasContextMenu() override { return true; }
            
            FInlineString GetDisplayName() const override
            {
                //@TODO this is pretty gross.
                FString IconString(LE_ICON_FOLDER);
                FString NameString(GetName().c_str());
                FString CombinedString(IconString + " " + NameString);

                return FInlineString(CombinedString.c_str());
            }

            const FName& GetName() const override
            {
                return Path.filename().string().c_str();
            }

            std::filesystem::path GetPath() const { return Path; }
            
        private:

            std::filesystem::path Path;
        };

        class FContentBrowserTileViewItem : public FTileViewItem
        {
        public:
            
            FContentBrowserTileViewItem(FTileViewItem* InParent, const std::filesystem::path& path)
                : FTileViewItem(InParent)
                , Path(std::move(path))
                , VirtualPath(Paths::ConvertToVirtualPath(path.string().c_str()))
            {}

            const char* GetTooltipText() const override { return "Test"; }
            bool HasContextMenu() override { return true; }

            FName GetName() const override
            {
                return Path.filename().stem().string().c_str();
            }

            std::filesystem::path GetPath() const { return Path; }
            FString GetVirtualPath() const { return VirtualPath; }
            
        private:

            std::filesystem::path   Path;
            FString                 VirtualPath;
        };

        LUMINA_SINGLETON_EDITOR_TOOL(FContentBrowserEditorTool)

        FContentBrowserEditorTool(IEditorToolContext* Context)
            : FEditorTool(Context, "Content Browser", nullptr)
            , OutlinerListView()
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


    private:

        void DrawDirectoryBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size);
        void DrawContentBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size);

        FTreeListView               OutlinerListView;
        FTreeListViewContext        OutlinerContext;

        FTileViewWidget             ContentBrowserTileView;
        FTileViewContext            ContentBrowserTileViewContext;

        std::filesystem::path       SelectedPath;
    
    };
}
