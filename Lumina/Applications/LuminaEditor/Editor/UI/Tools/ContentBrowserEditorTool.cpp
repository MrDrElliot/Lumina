#include "ContentBrowserEditorTool.h"

#include "EditorToolContext.h"
#include "Paths/Paths.h"

namespace Lumina
{
    void FContentBrowserEditorTool::OnInitialize(const FUpdateContext& UpdateContext)
    {
        CreateToolWindow("Content", [this] (const FUpdateContext& Contxt, bool bIsFocused)
        {
            float Left = 200.0f;
            float Right = ImGui::GetContentRegionAvail().x - Left;
            
            DrawDirectoryBrowser(Contxt, bIsFocused, ImVec2(Left, 0));
            ImGui::SameLine();
            DrawContentBrowser(Contxt, bIsFocused, ImVec2(Right, 0));
        });

        std::filesystem::path Path = Paths::GetEngineResourceDirectory();
        
        ContentBrowserTileViewContext.DrawItemContextMenuFunction = [this] (const TVector<FTileViewItem*> Item)
        {
            
        };

        ContentBrowserTileViewContext.RebuildTreeFunction = [this, Path] (FTileViewWidget* Tree)
        {
            for (auto& Directory : std::filesystem::directory_iterator(Path))
            {
                ContentBrowserTileView.AddItemToTree<FContentBrowserTileViewItem>(nullptr, Directory.path());
            }  
        };

        OutlinerContext.DrawItemContextMenuFunction = [this](const TVector<FTreeListViewItem*> Items)
        {
            for (FTreeListViewItem* Item : Items)
            {
                FContentBrowserListViewItem* EntityListItem = static_cast<FContentBrowserListViewItem*>(Item);
                
                
            }
        };
        
        OutlinerContext.RebuildTreeFunction = [this, Path](FTreeListView* Tree)
        {
            for (auto& Directory : std::filesystem::directory_iterator(Path))
            {
                OutlinerListView.AddItemToTree<FContentBrowserListViewItem>(nullptr, Directory.path());
            }
        };

        OutlinerListView.MarkTreeDirty();
        ContentBrowserTileView.MarkTreeDirty();
    }

    void FContentBrowserEditorTool::Update(const FUpdateContext& UpdateContext)
    {
        
    }

    void FContentBrowserEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID topDockID = 0, bottomLeftDockID = 0, bottomCenterDockID = 0, bottomRightDockID = 0;
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.5f, &bottomCenterDockID, &topDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.66f, &bottomCenterDockID, &bottomLeftDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.5f, &bottomRightDockID, &bottomCenterDockID);

        ImGui::DockBuilderDockWindow(GetToolWindowName("Content").c_str(), bottomCenterDockID);
    }

    void FContentBrowserEditorTool::DrawDirectoryBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size)
    {
        ImGui::BeginChild("Directories", Size);
        
        OutlinerListView.Draw(OutlinerContext);
        
        ImGui::EndChild();
    }

    void FContentBrowserEditorTool::DrawContentBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size)
    {
        std::filesystem::path Path = Paths::GetEngineResourceDirectory();

        ImGui::BeginChild("Content", Size);

        ContentBrowserTileView.Draw(ContentBrowserTileViewContext);

        ImGui::EndChild();

    }

}
