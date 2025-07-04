#include "ContentBrowserEditorTool.h"

#include "EditorToolContext.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/Definition/AssetDefinition.h"
#include "Assets/Factories/Factory.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/UI/ImGui/ImGuiMemoryEditor.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{

    void FContentBrowserEditorTool::RefreshContentBrowser()
    {
        ContentBrowserTileView.MarkTreeDirty();
        OutlinerListView.MarkTreeDirty();
        //GEngine->GetEngineSubsystem<FAssetRegistry>()->UpdateAssetDictionary();
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
        
        SelectedPath = FProject::Get()->GetProjectContentDirectory().c_str();
        
        ContentBrowserTileViewContext.ItemSelectedFunction = [this] (FTileViewItem* Item)
        {
            FContentBrowserTileViewItem* ContentItem = static_cast<FContentBrowserTileViewItem*>(Item);
            std::filesystem::path FSPath = ContentItem->GetPath();
            if (is_directory(FSPath))
            {
                SelectedPath = FSPath;
                RefreshContentBrowser();
            }
            else
            {
                FString ObjectName = ContentItem->GetName().ToString();
                FString QualifiedName = ContentItem->GetVirtualPath() + "." + ObjectName;

                CObject* Asset = LoadObject<CObject>(FName(QualifiedName));
                ToolContext->OpenAssetEditor(Asset);
            }
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
                                if (std::filesystem::exists(NewPath))
                                {
                                    throw std::filesystem::filesystem_error(
                                        "Destination path already exists",
                                        std::make_error_code(std::errc::file_exists)
                                    );
                                }
                                
                                std::filesystem::rename(OldPath, NewPath);

                                if (!std::filesystem::is_directory(OldPath))
                                {
                                    
                                }
                                
                                memset(Buf, 0, sizeof(Buf));
                                RefreshContentBrowser();
                                return true;
                            }
                            catch (const std::filesystem::filesystem_error& e)
                            {
                                LOG_ERROR("Failed to rename file: {0}", e.what());
                                return true;
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
            {
                return;
            }

            auto AddChildrenRecursive = [&](auto& AddChildrenRecursive,
                FContentBrowserListViewItem* ParentItem, const std::filesystem::path& ParentPath) -> void
            {
                for (auto& Entry : std::filesystem::directory_iterator(ParentPath))
                {
                    if (!std::filesystem::is_directory(Entry))
                        continue;

                    auto* ChildItem = ParentItem->AddChild<FContentBrowserListViewItem>(ParentItem, Entry.path());

                    AddChildrenRecursive(AddChildrenRecursive, ChildItem, Entry.path());
                    
                    if (Entry.path() == SelectedPath)
                    {
                        OutlinerListView.SetSelection(ChildItem, OutlinerContext);
                    }
                }
            };

            for (auto& Directory : std::filesystem::directory_iterator(Path))
            {
                if (!std::filesystem::is_directory(Directory))
                    continue;

                auto* RootItem = OutlinerListView.AddItemToTree<FContentBrowserListViewItem>(nullptr, Directory.path());

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
        bool bWroteSomething = false;

        ImVec2 AdjustedSize = ImVec2(Size.x - 2 * Padding, Size.y - 2 * Padding);

        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(Padding, Padding));

        ImGui::BeginChild("Content", AdjustedSize, true, ImGuiWindowFlags_None);

        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ContentContextMenu");
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 100.0f), ImVec2(0.0f, 0.0f));

        auto MakeUniquePath = [this](FString& InPath)
        {
            using namespace std::filesystem;

            FString BaseName = InPath;
            FString Extension;

            // Separate extension if it exists (optional)
            size_t DotIndex = InPath.find_last_of('.');
            if (DotIndex != FString::npos && is_regular_file(InPath.c_str()))
            {
                Extension = InPath.substr(DotIndex);
                BaseName = InPath.substr(0, DotIndex);
            }

            // Extract numeric suffix if it exists
            int SuffixNumber = 1;
            size_t UnderscorePos = BaseName.find_last_of('_');
            if (UnderscorePos != FString::npos)
            {
                FString SuffixPart = BaseName.substr(UnderscorePos + 1);
                char* EndPtr = nullptr;
                long ParsedNumber = std::strtol(SuffixPart.c_str(), &EndPtr, 10);

                if (EndPtr != SuffixPart.c_str() && *EndPtr == '\0') // Valid number
                {
                    SuffixNumber = static_cast<int>(ParsedNumber) + 1;
                    BaseName = BaseName.substr(0, UnderscorePos); // Strip old suffix
                }
            }

            FString CandidatePath = BaseName + "_" + eastl::to_string(SuffixNumber) + Extension;

            while (exists(CandidatePath.c_str()))
            {
                ++SuffixNumber;
                CandidatePath = BaseName + "_" + eastl::to_string(SuffixNumber) + Extension;
            }

            InPath = CandidatePath;
        };
        
        if (ImGui::BeginPopup("ContentContextMenu"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 5.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));
            
            {
                const char* FolderIcon = LE_ICON_FOLDER;
                FString MenuItemName = FString(FolderIcon) + " " + "New Folder";
                if (ImGui::MenuItem(MenuItemName.c_str()))
                {
                    std::filesystem::path NewPath = SelectedPath / "NewFolder";
                    FString PathString = NewPath.generic_string().c_str();
                    MakeUniquePath(PathString);
                    std::filesystem::create_directory(PathString.c_str());
                    bWroteSomething = true;
                }
            }

            ImGui::Separator();

            {
                const char* ImportIcon = LE_ICON_FILE_IMPORT;
                FString MenuItemName = FString(ImportIcon) + " " + "Import Asset";
                if (ImGui::MenuItem(MenuItemName.c_str()) &&  !FileBrowser.IsOpened())
                {
                    FileBrowser = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc);
                    FileBrowser.SetTitle("Select a file to import.");
                    FileBrowser.SetTypeFilters({".png", ".jpg", ".fbx", ".gltf"});

                    FileBrowser.Open();
                }
            }
            
            
            const char* FileIcon = LE_ICON_FILE;
            const char* File = "New Asset";

            FString MenuItemName = FString(FileIcon) + " " + File;

            if (ImGui::BeginMenu(MenuItemName.c_str()))
            {
                TVector<CAssetDefinition*> Definitions;
                CAssetDefinitionRegistry::Get()->GetAssetDefinitions(Definitions);
                for (CAssetDefinition* Definition : Definitions)
                {
                    if (Definition->CanImport())
                    {
                        continue;
                    }
                    
                    FString DisplayName = Definition->GetAssetDisplayName();
                    if (ImGui::MenuItem(DisplayName.c_str()))
                    {
                        CFactory* Factory = Definition->GetFactory();
                        FString PathString = Paths::Combine(SelectedPath.generic_string().c_str(), Factory->GetDefaultAssetCreationName(PathString).c_str());
                        Paths::AddPackageExtension(PathString);
                        MakeUniquePath(PathString);
                        PathString = Paths::RemoveExtension(PathString);
                        
                        CObject* Object = Factory->TryCreateNew(PathString);
                        ToolContext->OpenAssetEditor(Object);
                        bWroteSomething = true;

                    }
                }
                
                ImGui::EndMenu();
            }
            
            ImGui::PopStyleVar(2);
            ImGui::EndPopup();
        }
        
        FileBrowser.Display();
        if (FileBrowser.HasSelected())
        {
            std::filesystem::path FilePath = FileBrowser.GetSelected();
            FileBrowser.ClearSelected();
            FileBrowser.Close();
            
            TVector<CAssetDefinition*> Definitions;
            CAssetDefinitionRegistry::Get()->GetAssetDefinitions(Definitions);
            for (CAssetDefinition* Definition : Definitions)
            {
                if (!Definition->CanImport())
                {
                    continue;
                }

                if (Definition->GetImportFileExtension() != FilePath.extension().generic_string().c_str())
                {
                    continue;
                }
                
                CFactory* Factory = Definition->GetFactory();
                
                FString NoExtFileName = Paths::RemoveExtension(FilePath.filename().generic_string().c_str());
                FString PathString = Paths::Combine(SelectedPath.generic_string().c_str(), NoExtFileName.c_str());
                
                Paths::AddPackageExtension(PathString);
                MakeUniquePath(PathString);
                PathString = Paths::RemoveExtension(PathString);

                FTaskSystem::Get()->ScheduleLambda([this, Factory, FilePath, PathString]
                {
                    Factory->TryImport(FilePath.generic_string().c_str(), PathString);
                    RefreshContentBrowser();
                });
            }
            
            
            bWroteSomething = true;
        }
        
        if (bWroteSomething)
        {
            RefreshContentBrowser();
        }

        ContentBrowserTileView.Draw(ContentBrowserTileViewContext);
        ImGui::EndChild();
    
    }
}
