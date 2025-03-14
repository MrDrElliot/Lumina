#include "ContentBrowserEditorTool.h"

#include "EditorToolContext.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
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

    void FContentBrowserEditorTool::OnInitialize()
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

                            
                            if (!std::filesystem::is_directory(OldPath))
                            {
                                NewPath += OldPath.extension();
                            }
            
                            try
                            {
                                std::filesystem::rename(OldPath, NewPath);
                                memset(Buf, 0, sizeof(Buf));
                                RefreshContentBrowser();
                                return true;
                            }
                            catch (const std::filesystem::filesystem_error& e)
                            {
                                LOG_ERROR("Failed to rename file: {0}", e.what());
                            }
                            
                        }

                        return false;
                    });
                }


                if (ImGui::MenuItem("Delete"))
                {
                    try
                    {
                        std::filesystem::remove(ContentItem->GetPath().c_str());
                        RefreshContentBrowser();
                    }
                    catch (const std::filesystem::filesystem_error& e)
                    {
                        LOG_ERROR("Failed to delete file: {0}", e.what());
                    }
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

            if (!std::filesystem::exists(Path))
                return;

            // Recursive lambda for traversing directories
            auto AddChildrenRecursive = [&](auto& AddChildrenRecursive, FContentBrowserListViewItem* ParentItem, const std::filesystem::path& ParentPath) -> void
            {
                for (auto& Entry : std::filesystem::directory_iterator(ParentPath))
                {
                    if (!std::filesystem::is_directory(Entry))  // Skip files
                        continue;

                    auto* ChildItem = ParentItem->AddChild<FContentBrowserListViewItem>(ParentItem, Entry.path());

                    // Recurse into the subdirectory
                    AddChildrenRecursive(AddChildrenRecursive, ChildItem, Entry.path());
                }
            };

            // Process root directories
            for (auto& Directory : std::filesystem::directory_iterator(Path))
            {
                if (!std::filesystem::is_directory(Directory))  // Skip files
                    continue;

                auto* RootItem = OutlinerListView.AddItemToTree<FContentBrowserListViewItem>(nullptr, Directory.path());

                // Start recursion for subdirectories
                AddChildrenRecursive(AddChildrenRecursive, RootItem, Directory.path());
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

        ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 100.0f), ImVec2(0.0f, 0.0f));

        if (ImGui::BeginPopup("ContentContextMenu"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 5.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));

            {
                const char* FolderIcon = LE_ICON_FOLDER;
                const char* Folder = "New Folder";
            
                FString MenuItemName = FString(FolderIcon) + " " + Folder;
                if (ImGui::MenuItem(MenuItemName.c_str()))
                {
                    std::filesystem::path NewPath = SelectedPath / "NewFolder";
                    std::filesystem::create_directory(NewPath);
                    RefreshContentBrowser();
                }
            }

            ImGui::Separator();
            
            {
                const char* FileIcon = LE_ICON_FILE;
                const char* File = "New Asset";

                FString MenuItemName = FString(FileIcon) + " " + File;

                if (ImGui::BeginMenu(MenuItemName.c_str()))
                {
                    FFactoryRegistry* Registry = Context.GetSubsystem<FAssetManager>()->GetFactoryRegistry();

                    for (FFactory* Factory : Registry->GetFactories())
                    {
                        if (ImGui::MenuItem(Factory->GetAssetName().c_str()))
                        {
                            FString StringPath(SelectedPath.string().c_str());
                            FString NewFileName(StringPath + "/" + "NewMaterial.lasset");
                            if (Factory->CreateNew(NewFileName) != FAssetPath())
                            {
                                ToolContext->OpenAssetPath(NewFileName);
                                RefreshContentBrowser();
                            }
                        }
                    }

                    ImGui::EndMenu();
                }

                ImGui::PopStyleVar(2);
                ImGui::EndPopup();
            }
        }

        ContentBrowserTileView.Draw(ContentBrowserTileViewContext);

        ImGui::EndChild();
    }

    
}
