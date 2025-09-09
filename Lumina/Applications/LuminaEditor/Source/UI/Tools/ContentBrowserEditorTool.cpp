#include "ContentBrowserEditorTool.h"

#include "EditorToolContext.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/Definition/AssetDefinition.h"
#include "Assets/Factories/Factory.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/ObjectIterator.h"
#include "Core/Object/ObjectRedirector.h"
#include "Core/Object/Package/Package.h"
#include "EASTL/sort.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "Renderer/RenderManager.h"
#include "TaskSystem/TaskSystem.h"
#include "thumbnails/thumbnailmanager.h"
#include "Tools/Dialogs/Dialogs.h"
#include "Tools/Import/ImportHelpers.h"
#include "Tools/UI/UITextureCache.h"
#include "Tools/UI/ImGui/ImGuiMemoryEditor.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "UI/EditorUI.h"

namespace Lumina
{

    class FRenameModalState
    {
    public:
        
        char Buffer[256] = {};
        bool bInitialized = false;
    
        void Initialize(const FString& CurrentName)
        {
            if (!bInitialized)
            {
                strncpy_s(Buffer, sizeof(Buffer), CurrentName.c_str(), _TRUNCATE);
                bInitialized = true;
            }
        }
    
        void Reset()
        {
            memset(Buffer, 0, sizeof(Buffer));
            bInitialized = false;
        }
    };
    
    void FContentBrowserEditorTool::RefreshContentBrowser()
    {
        ContentBrowserTileView.MarkTreeDirty();
        OutlinerListView.MarkTreeDirty();
    }

    void FContentBrowserEditorTool::OnInitialize()
    {
        using namespace Import::Textures;

        for (TObjectIterator<CFactory> It; It; ++It)
        {
            CFactory* Factory = *It;
            if (Factory->HasAnyFlag(OF_DefaultObject))
            {
                if (CClass* AssetClass = Factory->GetSupportedType())
                {
                    FilterState.emplace(AssetClass->GetName().c_str(), true);
                }
            }
        }

        FilterState.emplace(CObjectRedirector::StaticClass()->GetName().c_str(), false);
        
        CreateToolWindow("Content", [this] (const FUpdateContext& Contxt, bool bIsFocused)
        {
            float Left = 200.0f;
            float Right = ImGui::GetContentRegionAvail().x - Left;
            
            DrawDirectoryBrowser(Contxt, bIsFocused, ImVec2(Left, 0));
            
            ImGui::SameLine();

            DrawContentBrowser(Contxt, bIsFocused, ImVec2(Right, 0));
        });
        
        SelectedPath = FProject::Get().GetProjectContentDirectory().c_str();

        GEngine->GetEngineSubsystem<FAssetRegistry>()->GetOnAssetRegistryUpdated().AddTFunction([this]
        {
            RefreshContentBrowser();
        });
        
        ContentBrowserTileViewContext.DragDropFunction = [this] (FTileViewItem* DropItem)
        {
            auto* TypedDroppedItem = (FContentBrowserTileViewItem*)DropItem;
            if (!TypedDroppedItem->IsDirectory())
            {
                return;
            }
            
            const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(FContentBrowserTileViewItem::DragDropID, ImGuiDragDropFlags_AcceptBeforeDelivery);
            if (Payload && Payload->IsDelivery())
            {
                uintptr_t* RawPtr = (uintptr_t*)Payload->Data;
                auto* SourceItem = (FContentBrowserTileViewItem*)*RawPtr;

                if (SourceItem == TypedDroppedItem)
                {
                    return;
                }

                HandleContentBrowserDragDrop(TypedDroppedItem, SourceItem);
                
            }
        };

        ContentBrowserTileViewContext.DrawItemOverrideFunction = [this] (FTileViewItem* Item) -> bool
        {
            FContentBrowserTileViewItem* ContentItem = static_cast<FContentBrowserTileViewItem*>(Item);

            ImTextureRef ImTexture = GetEngineSystem<FRenderManager>().GetTextureCache()->GetImTexture(Paths::GetEngineResourceDirectory() + "/Textures/Folder.png");

            if ((!ContentItem->IsDirectory()))
            {
                if (ContentItem->GetAssetData().IsRedirector())
                {
                    ImTexture = GetEngineSystem<FRenderManager>().GetTextureCache()->GetImTexture(Paths::GetEngineResourceDirectory() + "/Textures/Redirect.png");
                }
                else
                {
                    ImTexture = GetEngineSystem<FRenderManager>().GetTextureCache()->GetImTexture(Paths::GetEngineResourceDirectory() + "/Textures/SkeletalMeshIcon.png");
                }
            }
            
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.05f)); 
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.5f));

            ImGui::ImageButton("##", ImTexture, ImVec2(128.0f, 128.0f));

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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
            if (ContentItem->IsDirectory())
            {
                SelectedPath = ContentItem->GetPath();
                RefreshContentBrowser();
            }
            else
            {
                FString QualifiedName = ContentItem->GetAssetData().FullPath.ToString();

                CObject* Asset = LoadObject<CObject>(nullptr, QualifiedName);
                ToolContext->OpenAssetEditor(Asset);
            }
        };
        
        ContentBrowserTileViewContext.DrawItemContextMenuFunction = [this] (const TVector<FTileViewItem*>& Items)
        {
            for (FTileViewItem* Item : Items)
            {
                FContentBrowserTileViewItem* ContentItem = static_cast<FContentBrowserTileViewItem*>(Item);

                if (!ContentItem->IsDirectory())
                {
                    if (ContentItem->GetAssetData().IsRedirector())
                    {
                        if (ImGui::MenuItem("Fixup Redirectors"))
                        {
                            
                        }   
                    }
                    else
                    {
                        if (ImGui::MenuItem("Open"))
                        {
                            FString QualifiedName = ContentItem->GetAssetData().FullPath.ToString();

                            CObject* Asset = LoadObject<CObject>(nullptr, QualifiedName);
                            ToolContext->OpenAssetEditor(Asset);
                        }   
                    }
                }
                
                if (ImGui::MenuItem("Rename"))
                {
                    auto RenameState = MakeSharedPtr<FRenameModalState>();
            
                    ToolContext->PushModal("Rename", ImVec2(275.0f, 155.0f), [this, ContentItem, RenameState](const FUpdateContext&) -> bool
                    {
                        RenameState->Initialize(ContentItem->GetFileName());

                        ImGui::SetKeyboardFocusHere();
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        
                        bool bSubmitted = ImGui::InputText("##", RenameState->Buffer, sizeof(RenameState->Buffer), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);

                        
                        if (!ContentItem->IsDirectory())
                        {
                            FString TestAssetName = ContentItem->GetAssetData().PackageName.ToString() + "." + RenameState->Buffer;
                            if (CObject* TestObject = FindObject<CObject>(nullptr, TestAssetName))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 55, 55, 255));
                                ImGui::TextWrapped("Already Exists: %s", *TestObject->GetQualifiedName());
                                ImGui::PopStyleColor();
                                
                                if (ImGui::Button("Cancel"))
                                {
                                    return true;
                                }

                                return false;
                            }
                        }
                        
                        if (bSubmitted && strlen(RenameState->Buffer) > 0)
                        {
                            FString NewPath = Paths::Parent(ContentItem->GetPath()) + "/" + RenameState->Buffer;

                            if (HandleRenameEvent(ContentItem->GetPath(), NewPath))
                            {
                                if (ContentItem->IsDirectory())
                                {
                                    RefreshContentBrowser();
                                }
                            }
                    
                            RenameState->Reset();
                            return true;
                        }

                        if (ImGui::Button("Cancel"))
                        {
                            return true;
                        }
                        
                        return false;
                    });
                }


                if (ImGui::MenuItem(LE_ICON_ALERT_OCTAGON" Delete"))
                {
                    if (Dialogs::Confirmation("Delete", "Are you sure you wish to delete this file?"))
                    {
                        try
                        {
                            FString PackagePath = ContentItem->GetPath();

                            bool bWasSuccessful = false;

                            if (std::filesystem::is_directory(PackagePath.c_str()))
                            {
                                std::filesystem::remove(PackagePath.c_str());
                                bWasSuccessful = true;
                                RefreshContentBrowser();
                            }
                            else
                            {
                                FString ObjectName = ContentItem->GetFileName();
                                FString QualifiedName = ContentItem->GetVirtualPath() + "." + ObjectName;

                                if (CObject* AliveObject = FindObject<CObject>(nullptr, QualifiedName))
                                {
                                    ToolContext->OnDestroyAsset(AliveObject);
                                }
                        
                                if (CPackage::DestroyPackage(PackagePath))
                                {
                                    FString PackagePathWithExt = PackagePath + ".lasset";
                                    std::filesystem::remove(PackagePathWithExt.c_str());
                                    bWasSuccessful = true;
                                }
                            }

                            if (bWasSuccessful)
                            {
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
            }
        };

        ContentBrowserTileViewContext.RebuildTreeFunction = [this] (FTileViewWidget* Tree)
        {
            if (Paths::Exists(SelectedPath))
            {
                TFixedVector<FString, 24> DirectoryPaths;
                
                for (auto& Directory : std::filesystem::directory_iterator(SelectedPath.c_str()))
                {
                    if (std::filesystem::is_directory(Directory))
                    {
                        FString VirtualPath = Paths::ConvertToVirtualPath(Directory.path().generic_string().c_str());
                        DirectoryPaths.push_back(Directory.path().generic_string().c_str());
                    }
                }

                eastl::sort(DirectoryPaths.begin(), DirectoryPaths.end());
                
                for (const FString& Directory : DirectoryPaths)
                {
                    ContentBrowserTileView.AddItemToTree<FContentBrowserTileViewItem>(nullptr, Directory, nullptr);
                }

                FString FullPath = Paths::ConvertToVirtualPath(SelectedPath);
                TVector<FAssetData*> Assets = GEngine->GetEngineSubsystem<FAssetRegistry>()->GetAssetsForPath(FullPath);

                eastl::sort(Assets.begin(), Assets.end(), [](const FAssetData* A, const FAssetData* B)
                {
                    return A->AssetName.ToString() > B->AssetName.ToString();
                });
                
                for (FAssetData* Asset : Assets)
                {
                    FName ShortClassName = Paths::GetExtension(Asset->AssetClass.ToString());
                    if (FilterState.at(ShortClassName))
                    {
                        FullPath = Paths::ResolveVirtualPath(Asset->FullPath.ToString());
                        ContentBrowserTileView.AddItemToTree<FContentBrowserTileViewItem>(nullptr, FullPath, Asset);
                    }
                }
            }
        };

        OutlinerContext.DrawItemContextMenuFunction = [this](const TVector<FTreeListViewItem*>& Items)
        {
            for (FTreeListViewItem* Item : Items)
            {
                FContentBrowserListViewItem* ContentItem = static_cast<FContentBrowserListViewItem*>(Item);
                
            }
        };
        
        OutlinerContext.RebuildTreeFunction = [this](FTreeListView* Tree)
        {
            for (const auto& [VirtualPrefix, PhysicalRootStr] : Paths::GetMountedPaths())
            {
                auto* RootItem = OutlinerListView.AddItemToTree<FContentBrowserListViewItem>(nullptr, PhysicalRootStr, VirtualPrefix.ToString());

                TFunction<void(FContentBrowserListViewItem*, const FString&)> AddChildrenRecursive;

                AddChildrenRecursive = [&](FContentBrowserListViewItem* ParentItem, const FString& CurrentPath)
                {
                    std::error_code ec;
                    for (auto& Entry : std::filesystem::directory_iterator(CurrentPath.c_str(), ec))
                    {
                        if (ec || !Entry.is_directory())
                            continue;

                        FString Path = Entry.path().generic_string().c_str();
                        FString DisplayName = Entry.path().filename().string().c_str();
                        auto* ChildItem = ParentItem->AddChild<FContentBrowserListViewItem>(ParentItem, Path, DisplayName);

                        if (Entry.path() == SelectedPath.c_str())
                        {
                            OutlinerListView.SetSelection(ChildItem, OutlinerContext);
                        }

                        AddChildrenRecursive(ChildItem, Path);
                    }
                };

                AddChildrenRecursive(RootItem, PhysicalRootStr);
            }
        };



        OutlinerContext.ItemSelectedFunction = [this] (FTreeListViewItem* Item)
        {
            if (Item == nullptr)
            {
                return;
            }
            
            FContentBrowserListViewItem* ContentItem = static_cast<FContentBrowserListViewItem*>(Item);
            
            SelectedPath = ContentItem->GetPath();

            RefreshContentBrowser();
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

    void FContentBrowserEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::BeginMenu(LE_ICON_FILTER " Filter"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 2));

            for (auto& [Name, State] : FilterState)
            {
                if (ImGui::Checkbox(Name.c_str(), &State))
                {
                    RefreshContentBrowser();
                }
            }

            ImGui::PopStyleVar(2);
            ImGui::EndMenu();
        }
    }

    void FContentBrowserEditorTool::HandleContentBrowserDragDrop(FContentBrowserTileViewItem* Drop, FContentBrowserTileViewItem* Payload)
    {
        bool bWroteSomething = false;
        
        /** If the payload is a folder, and is empty, we don't need to do much besides moving the folder */
        if (Payload->IsDirectory())
        {
            if (std::filesystem::is_empty(Payload->GetPath().c_str()))
            {
                std::filesystem::path SourcePath = Payload->GetPath().c_str();
                FString SourceFileName = SourcePath.filename().generic_string().c_str();
            
                FString DestPath = Drop->GetPath() + "/" + SourceFileName;
                
                try
                {
                    std::filesystem::rename(SourcePath, DestPath.c_str());
                    LOG_INFO("[ContentBrowser] Moved folder {0} -> {1}", SourcePath.string(), DestPath);
                    bWroteSomething = true;
                }
                catch (const std::filesystem::filesystem_error& e)
                {
                    LOG_ERROR("[ContentBrowser] Failed to move folder: {0}", e.what());
                }
            }
            else
            {
                //... Moving all assets in a folder :(
            }
        }


        if (!Payload->IsDirectory())
        {
            FString OldPath = Payload->GetPath();
            FString NewPath = Drop->GetPath() + "/" + Payload->GetAssetData().AssetName.ToString();
            
            HandleRenameEvent(OldPath, NewPath);
        }
    }

    bool FContentBrowserEditorTool::HandleRenameEvent(const FString& OldPath, FString NewPath)
    {
        try
        {
            bool bDirectory = std::filesystem::is_directory(OldPath.c_str());
            
            if (!bDirectory)
            {
                NewPath += ".lasset";
            }
        
            if (std::filesystem::exists(NewPath.c_str()))
            {
                LOG_ERROR("Destination path already exists: {0}", NewPath);
                return false;
            }
            
            if (!bDirectory)
            {
                if (CPackage* OldPackage = CPackage::LoadPackage(OldPath.c_str()))
                {
                    // Make sure nothing is currently loading before executing this rename.
                    GEngine->GetEngineSubsystem<FAssetManager>()->FlushAsyncLoading();
                    
                    /** We need all objects to be loaded to rename a package */
                    OldPackage->LoadObjects();

                    bool bCreateRedirectors = true;
                    
                    FAssetRegistry* AssetRegistry = GEngine->GetEngineSubsystem<FAssetRegistry>();
                    const THashSet<FName>& References = AssetRegistry->GetReferences(OldPackage->GetName());

                    //@TODO bCreateRedirectors = References.empty();
                    
                    FString OldAssetName = Paths::FileName(OldPath, true);
                    FString NewAssetName = Paths::FileName(NewPath, true);

                    CPackage* NewPackage = CPackage::CreatePackage(NewPath);
                    
                    TVector<CObject*> Objects;
                    GetObjectsWithPackage(OldPackage, Objects);

                    for (CObject* Object : Objects)
                    {
                        if (Object->IsAsset())
                        {
                            Object->Rename(NewAssetName, NewPackage, bCreateRedirectors);
                            AssetRegistry->AssetRenamed(Object, OldPath);
                        }
                        else
                        {
                            Object->Rename(Object->GetName(), NewPackage);
                        }
                    }

                    CPackage::SavePackage(OldPackage, nullptr, OldPath);
                    CPackage::SavePackage(NewPackage, nullptr, NewPath);
                }
            }
            else
            {
                std::filesystem::rename(OldPath.c_str(), NewPath.c_str());
            }
            
            return true;
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            LOG_ERROR("Failed to rename file: {0}", e.what());
            return false;
        }
    }
    
    void FContentBrowserEditorTool::DrawDirectoryBrowser(const FUpdateContext& Contxt, bool bIsFocused, ImVec2 Size)
    {
        ImGui::BeginChild("Directories", Size);

        OutlinerListView.Draw(OutlinerContext);
        
        ImGui::EndChild();
    }

    void FContentBrowserEditorTool::DrawContentBrowser(const FUpdateContext& Context, bool bIsFocused, ImVec2 Size)
    {
        FString Path = Paths::GetEngineResourceDirectory();
        constexpr float Padding = 10.0f;

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
                    std::filesystem::path NewPath = FString(SelectedPath + "/NewFolder").c_str();
                    FString PathString = NewPath.generic_string().c_str();
                    MakeUniquePath(PathString);
                    std::filesystem::create_directory(PathString.c_str());
                    RefreshContentBrowser();
                }
            }
            
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
            
            
            const char* FileIcon = LE_ICON_PLUS;
            const char* File = "New Asset";

            ImGui::Separator();
            
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
                        FString PathString = Paths::Combine(SelectedPath.c_str(), Factory->GetDefaultAssetCreationName(PathString).c_str());
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
                    FString PathString = Paths::Combine(SelectedPath.c_str(), NoExtFileName.c_str());
                
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
                                ImGuiX::Notifications::NotifySuccess("Successfully Imported: \"%s\"", PathString.c_str());
                            }
                    
                            return bShouldClose;
                        });
                    }
                    else
                    {
                        Task::AsyncTask(1, [this, Factory, FStringFileName, PathString] (uint32 Start, uint32 End, uint32 ThreadNum_)
                        {
                            Factory->TryImport(FStringFileName, PathString);
                            ImGuiX::Notifications::NotifySuccess("Successfully Imported: \"%s\"", PathString.c_str());
                        });
                    }
                }
            }
        }

        ImGui::BeginHorizontal("Breadcrumbs");

        std::filesystem::path SelectedFSPath = SelectedPath.c_str();
        std::filesystem::path current;
        bool bStarted = false;

        for (auto it = SelectedFSPath.begin(); it != SelectedFSPath.end(); ++it)
        {
            current /= *it;

            if (!bStarted)
            {
                if (it->string() == "Content")
                {
                    bStarted = true;
                }
                else
                {
                    continue;
                }
            }

            ImGui::PushID(std::distance(SelectedFSPath.begin(), it));
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));
                if (ImGui::Button(it->string().c_str()))
                {
                    SelectedPath = current.generic_string().c_str();
                    ContentBrowserTileView.MarkTreeDirty();
                }
                ImGui::PopStyleVar(2);
            }
            ImGui::PopID();

            auto next = it;
            ++next;
            
            if (next != SelectedFSPath.end())
            {
                ImGui::TextUnformatted(LE_ICON_ARROW_RIGHT);
            }
        }

        ImGui::EndHorizontal();

        ImGuiTextFilter SearchFilter;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
        SearchFilter.Draw("##Search", 450.0f);
        ImGui::PopStyleVar();

        ImGui::Separator();
        
        ContentBrowserTileView.Draw(ContentBrowserTileViewContext);
        
        ImGui::EndChild();
    
    }
}
