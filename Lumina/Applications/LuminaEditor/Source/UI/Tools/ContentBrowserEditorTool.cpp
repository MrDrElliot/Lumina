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
#include "Renderer/RenderManager.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/Import/ImportHelpers.h"
#include "Tools/UI/ImGui/ImGuiMemoryEditor.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "UI/EditorUI.h"

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
        using namespace Import::Textures;


        CreateToolWindow("Content", [this] (const FUpdateContext& Contxt, bool bIsFocused)
        {
            float Left = 200.0f;
            float Right = ImGui::GetContentRegionAvail().x - Left;
            
            DrawDirectoryBrowser(Contxt, bIsFocused, ImVec2(Left, 0));
            ImGui::SameLine();
            DrawContentBrowser(Contxt, bIsFocused, ImVec2(Right, 0));
        });
        
        SelectedPath = FProject::Get()->GetProjectContentDirectory().c_str();

        ContentBrowserTileViewContext.DrawItemOverrideFunction = [this] (FTileViewItem* Item) -> bool
        {
            FContentBrowserTileViewItem* ContentItem = static_cast<FContentBrowserTileViewItem*>(Item);
            FString Path = ContentItem->GetPath().generic_string().c_str();
            ImTextureID ImTexture;
            
            FAssetRegistry* Registry = GEngine->GetEngineSubsystem<FAssetRegistry>();
            if (Registry->IsPathCorrupt(Path))
            {
                ImTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::CorruptIcon);
            }
            else
            {
                FAssetData Asset = Registry->GetAsset(Path);
                if (Asset.ClassName == "CMaterial")
                {
                    ImTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::MaterialIcon);
                }
                else if (Asset.ClassName == "CStaticMesh")
                {
                    ImTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::StaticMeshIcon);
                }
                else if (Asset.ClassName == "CTexture")
                {
                    ImTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::TextureIcon);
                }
                else
                {
                    ImTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::FolderIcon);
                }
            }
            

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.05f)); 
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.5f));
            if (ImGui::ImageButton("##", ImTexture, ImVec2(115.0f, 115.0f)))
            {
                ImGui::PopStyleColor(3);
                return true;
            }

            ImGui::PopStyleColor(3);

            return false;
        };
        
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
                        FString PackagePath = ContentItem->GetVirtualPath();
                        FString ObjectName = ContentItem->GetName().ToString();
                        FString QualifiedName = ContentItem->GetVirtualPath() + "." + ObjectName;

                        if (CObject* AliveObject = FindObject<CObject>(FName(QualifiedName)))
                        {
                            ToolContext->OnDestroyAsset(AliveObject);
                        }
                        
                        if (CPackage::DestroyPackage(PackagePath) && std::filesystem::remove(ContentItem->GetPath().c_str()))
                        {
                            RefreshContentBrowser();
                            ImGuiX::Notifications::NotifySuccess("Successfully deleted: \"%s\"", PackagePath.c_str());
                        }
                        else
                        {
                            ImGuiX::Notifications::NotifyError("Failed to delete: \"%s\"", PackagePath.c_str());
                        }
                    }
                    catch (const std::filesystem::filesystem_error& e)
                    {
                        LOG_ERROR("Failed to delete file: {0}", e.what());
                        ImGuiX::Notifications::NotifyError("Failed to delete: \"%s\"", e.what());
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
            if (Item == nullptr)
            {
                return;
            }
            
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

            size_t DotIndex = InPath.find_last_of('.');
            if (DotIndex != FString::npos && is_regular_file(InPath.c_str()))
            {
                Extension = InPath.substr(DotIndex);
                BaseName = InPath.substr(0, DotIndex);
            }

            int SuffixNumber = 1;
            size_t NameLen = BaseName.length();

            size_t NumberStart = NameLen;
            while (NumberStart > 0 && isdigit(BaseName[NumberStart - 1]))
            {
                --NumberStart;
            }

            if (NumberStart < NameLen)
            {
                FString SuffixPart = BaseName.substr(NumberStart);
                char* EndPtr = nullptr;
                long ParsedNumber = std::strtol(SuffixPart.c_str(), &EndPtr, 10);

                if (EndPtr != SuffixPart.c_str() && *EndPtr == '\0')
                {
                    SuffixNumber = static_cast<int>(ParsedNumber) + 1;
                    BaseName = BaseName.substr(0, NumberStart);
                }
            }

            FString CandidatePath = BaseName + eastl::to_string(SuffixNumber) + Extension;

            while (exists(CandidatePath.c_str()))
            {
                ++SuffixNumber;
                CandidatePath = BaseName + eastl::to_string(SuffixNumber) + Extension;
            }

            InPath = CandidatePath;
        };
        
        if (ImGui::BeginPopup("ContentContextMenu"))
        {
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
                    FileBrowser = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_MultipleSelection);
                    FileBrowser.SetTitle("Select a file(s) to import.");
                    FileBrowser.SetTypeFilters({".png", ".jpg", ".fbx", ".gltf", ".glb"});

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

                        if (Factory->HasCreationDialogue())
                        {
                            ToolContext->PushModal("Create New", {500, 500}, [this, Factory, PathString](const FUpdateContext& DrawContext)
                            {
                                bool bShouldClose = CFactory::ShowCreationDialogue(Factory, PathString);
                                if (bShouldClose)
                                {
                                    RefreshContentBrowser();
                                    ImGuiX::Notifications::NotifySuccess("Successfully Created: \"%s\"", PathString.c_str());
                                }
                        
                                return bShouldClose;
                            });
                        }
                        else
                        {
                            CObject* Object = Factory->TryCreateNew(PathString);
                            if (Object)
                            {
                                bWroteSomething = true;
                                ImGuiX::Notifications::NotifySuccess("Successfully Created: \"%s\"", PathString.c_str());
                            }
                            else
                            {
                                ImGuiX::Notifications::NotifyError("Failed to create new: \"%s\"", PathString.c_str());

                            }
                        }
                    }
                }
                
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
        
        FileBrowser.Display();
        if (FileBrowser.HasSelected())
        {
            std::vector<std::filesystem::path> Files = FileBrowser.GetMultiSelected();
            FileBrowser.ClearSelected();
            FileBrowser.Close();

            for (const auto& FilePath : Files)
            {
                TVector<CAssetDefinition*> Definitions;
                CAssetDefinitionRegistry::Get()->GetAssetDefinitions(Definitions);
                for (CAssetDefinition* Definition : Definitions)
                {
                    if (!Definition->CanImport())
                    {
                        continue;
                    }

                    FString Ext = FilePath.extension().generic_string().c_str();
                    if (!Definition->IsExtensionSupported(Ext))
                    {
                        continue;
                    }
                
                    CFactory* Factory = Definition->GetFactory();

                    FString FStringFileName = FilePath.generic_string().c_str();
                    FString NoExtFileName = Paths::RemoveExtension(FilePath.filename().generic_string().c_str());
                    FString PathString = Paths::Combine(SelectedPath.generic_string().c_str(), NoExtFileName.c_str());
                
                    Paths::AddPackageExtension(PathString);
                    MakeUniquePath(PathString);
                    PathString = Paths::RemoveExtension(PathString);

                    if (Factory->HasImportDialogue())
                    {
                        ToolContext->PushModal("Import", {500, 500}, [this, Factory, FStringFileName, PathString](const FUpdateContext& DrawContext)
                        {
                            bool bShouldClose = CFactory::ShowImportDialogue(Factory, FStringFileName, PathString);
                            if (bShouldClose)
                            {
                                RefreshContentBrowser();
                                ImGuiX::Notifications::NotifySuccess("Successfully Imported: \"%s\"", PathString.c_str());
                            }
                    
                            return bShouldClose;
                        });
                    }
                    else
                    {
                        FTaskSystem::Get()->ScheduleLambda(1, [this, Factory, FStringFileName, PathString] (uint32 Start, uint32 End, uint32 ThreadNum_)
                        {
                            Factory->TryImport(FStringFileName, PathString);
                            RefreshContentBrowser();
                            ImGuiX::Notifications::NotifySuccess("Successfully Imported: \"%s\"", PathString.c_str());
                        });
                    }
                }
            }
        }
        
        if (bWroteSomething)
        {
            RefreshContentBrowser();
        }

        ContentBrowserTileView.Draw(ContentBrowserTileViewContext);
        ImGui::EndChild();
    
    }
}
