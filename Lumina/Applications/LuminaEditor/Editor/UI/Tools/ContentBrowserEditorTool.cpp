#include "ContentBrowserEditorTool.h"

#include "EditorToolContext.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{

    void FContentBrowserEditorTool::RefreshContentBrowser()
    {
        ContentBrowserTileView.MarkTreeDirty();
        OutlinerListView.MarkTreeDirty();
    }

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

        
        std::filesystem::path Path = FProject::Get()->GetProjectSettings().ProjectPath.c_str();
        Path = Path.parent_path() / "Game/Content";
        SelectedPath = Path;
        
        ContentBrowserTileViewContext.ItemSelectedFunction = [this] (FTileViewItem* Item)
        {
            
        };
        
        ContentBrowserTileViewContext.DrawItemContextMenuFunction = [this] (const TVector<FTileViewItem*> Items)
        {
            for (FTileViewItem* Item : Items)
            {
                FContentBrowserTileViewItem* ContentItem = static_cast<FContentBrowserTileViewItem*>(Item);
                
                if (ImGui::MenuItem("Rename"))
                {
                    ToolContext->PushModal("Rename", ImVec2(350.0f, 100.0f), [this, ContentItem](const FUpdateContext& Context) -> bool
                    {
                        static char Buf[256] = {};

                        if (Buf[0] == '\0')
                        {
                            memset(Buf, 0, sizeof(Buf));
                            strncpy(Buf, ContentItem->GetName().c_str(), sizeof(Buf) - 1);
                            Buf[sizeof(Buf) - 1] = '\0';
                        }

                        ImGui::SetKeyboardFocusHere();
                        bool bSubmitted = ImGui::InputText("New Name", Buf, sizeof(Buf),
                            ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
        
                        if (bSubmitted)
                        {
                            std::filesystem::path OldPath = ContentItem->GetPath();
                            std::filesystem::path NewPath = OldPath.parent_path() / Buf;
            
                            try
                            {
                                std::filesystem::rename(OldPath, NewPath);
                                memset(Buf, 0, sizeof(Buf));
                                RefreshContentBrowser();
                                return true;
                            }
                            catch (const std::filesystem::filesystem_error& e)
                            {
                                LOG_ERROR("Failed to rename file: {}", e.what());
                            }
                            
                        }

                        return false;
                    });
                }


                if (ImGui::MenuItem("Delete"))
                {
                    
                }
            }
        };

        ContentBrowserTileViewContext.RebuildTreeFunction = [this] (FTileViewWidget* Tree)
        {
            if (std::filesystem::exists(SelectedPath))
            {
                for (auto& Directory : std::filesystem::directory_iterator(SelectedPath))
                {
                    ContentBrowserTileView.AddItemToTree<FContentBrowserTileViewItem>(nullptr, Directory.path());
                }
            }
        };

        OutlinerContext.DrawItemContextMenuFunction = [this](const TVector<FTreeListViewItem*> Items)
        {
            for (FTreeListViewItem* Item : Items)
            {
                FContentBrowserListViewItem* ContentItem = static_cast<FContentBrowserListViewItem*>(Item);
                
                
            }
        };
        
        OutlinerContext.RebuildTreeFunction = [this](FTreeListView* Tree)
        {
            std::filesystem::path Path = FProject::Get()->GetProjectSettings().ProjectPath.c_str();
            Path = Path.parent_path() / "Game/";
            
            if (std::filesystem::exists(Path))
            {
                for (auto& Directory : std::filesystem::recursive_directory_iterator(Path))
                {
                    OutlinerListView.AddItemToTree<FContentBrowserListViewItem>(nullptr, Directory.path());
                }
            }
        };

        OutlinerContext.ItemSelectedFunction = [this] (FTreeListViewItem* Item)
        {
            FContentBrowserListViewItem* ContentItem = static_cast<FContentBrowserListViewItem*>(Item);

            std::filesystem::path Path = ContentItem->GetPath();

            SelectedPath = Path;

            ContentBrowserTileView.MarkTreeDirty();
            
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

    void FContentBrowserEditorTool::DrawContentBrowser(const FUpdateContext& Context, bool bIsFocused, ImVec2 Size)
    {
        std::filesystem::path Path = Paths::GetEngineResourceDirectory();

        constexpr float Padding = 10.0f;

        ImVec2 AdjustedSize = ImVec2(Size.x - 2 * Padding, Size.y - 2 * Padding);

        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(Padding, Padding));

        ImGui::BeginChild("Content", AdjustedSize, true, ImGuiWindowFlags_None);

        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ContentContextMenu");
        }

        if (ImGui::BeginPopup("ContentContextMenu"))
        {
            if (ImGui::MenuItem("New Folder"))
            {
                std::filesystem::path NewPath = SelectedPath / "NewFolder";
                std::filesystem::create_directory(NewPath);
                RefreshContentBrowser();
            }
            
            if (ImGui::BeginMenu("New Asset"))
            {
                if (ImGui::MenuItem("Material"))
                {
                    
                }

                ImGui::EndMenu();
            }

            
            ImGui::EndPopup();
        }

        // Draw content (like tiles or buttons)
        ContentBrowserTileView.Draw(ContentBrowserTileViewContext);

        ImGui::EndChild();
    }

    
}
