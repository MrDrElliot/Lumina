#include "EditorUI.h"

#include "imgui.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Object/Class.h"
#include "Core/Object/Object.h"
#include "Memory/Memory.h"
#include "Project/Project.h"
#include "Renderer/RenderContext.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Systems/DebugCameraEntitySystem.h"
#include "Tools/ConsoleLogEditorTool.h"
#include "Tools/ContentBrowserEditorTool.h"
#include "Tools/EditorTool.h"
#include "Tools/EditorToolModal.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "Tools/EntitySceneEditorTool.h"
#include "Tools/AssetEditors/MaterialEditor/MaterialEditorTool.h"
#include "Tools/UI/ImGui/imfilebrowser.h"
#include "imnodes/imnodes.h"
#include <Assets/AssetHeader.h>
#include <client/TracyProfiler.hpp>

#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Object/Package/Package.h"
#include "Core/Profiler/Profile.h"
#include "Renderer/RenderManager.h"
#include "Scene/SceneRenderer.h"
#include "Tools/AssetEditors/MeshEditor/MeshEditorTool.h"
#include "Tools/AssetEditors/TextureEditor/TextureEditorTool.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    FEditorUI::FEditorUI()
    {
    }

    FEditorUI::~FEditorUI()
    {
    }

    void FEditorUI::Initialize(const FUpdateContext& UpdateContext)
    {
        ImNodes::CreateContext();
        
        EditorWindowClass.ClassId = ImHashStr("EditorWindowClass");
        EditorWindowClass.DockingAllowUnclassed = false;
        EditorWindowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
        EditorWindowClass.ViewportFlagsOverrideClear = ImGuiViewportFlags_NoDecoration | ImGuiViewportFlags_NoTaskBarIcon;
        EditorWindowClass.ParentViewportId = 0; // Top level window
        EditorWindowClass.DockingAllowUnclassed = false;
        EditorWindowClass.DockingAlwaysTabBar = true;


        SubsystemManager = UpdateContext.GetSubsystemManager();
        AssetRegistry = UpdateContext.GetSubsystem<FAssetRegistry>();
        SceneManager = UpdateContext.GetSubsystem<FSceneManager>();

        
        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
        NewScene->RegisterSystem(Memory::New<FDebugCameraEntitySystem>());
        
        SceneEditorTool = CreateTool<FEntitySceneEditorTool>(this, NewScene);
        ConsoleLogTool = CreateTool<FConsoleLogEditorTool>(this);
        ContentBrowser = CreateTool<FContentBrowserEditorTool>(this);

    }

    void FEditorUI::Deinitialize(const FUpdateContext& UpdateContext)
    {
        while (!EditorTools.empty())
        {
            DestroyTool(UpdateContext, EditorTools[0]);
        }

        SceneManager = nullptr;
        SceneEditorTool = nullptr;
        ConsoleLogTool = nullptr;

        ImNodes::DestroyContext();
    }

    void FEditorUI::OnStartFrame(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(UpdateContext.GetUpdateStage() == EUpdateStage::FrameStart)

        auto TitleBarLeftContents = [this, &UpdateContext] ()
        {
            DrawTitleBarMenu(UpdateContext);
        };

        auto TitleBarRightContents = [this, &UpdateContext] ()
        {
            DrawTitleBarInfoStats(UpdateContext);
        };

        TitleBar.Draw(TitleBarLeftContents, 400, TitleBarRightContents, 290);

        const ImGuiID DockspaceID = ImGui::GetID("EditorDockSpace");

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("EditorDockSpaceWindow", nullptr, WindowFlags);
        ImGui::PopStyleVar(3);
        {
            // Create initial layout
            if (!ImGui::DockBuilderGetNode(DockspaceID))
            {
                ImGui::DockBuilderAddNode(DockspaceID, ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(DockspaceID, ImGui::GetContentRegionAvail());

                // Create Dock IDs
                ImGuiID topDockID = 0, bottomDockID = 0;
                ImGuiID bottomLeftDockID = 0, bottomRightDockID = 0;

                // Split main dock into top (scene editor) and bottom (console + renderer info)
                ImGui::DockBuilderSplitNode(DockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &topDockID);

                // Split bottom dock into left (console) and right (renderer info)
                ImGui::DockBuilderSplitNode(bottomDockID, ImGuiDir_Left, 0.5f, &bottomLeftDockID, &bottomRightDockID);

                ImGui::DockBuilderFinish(DockspaceID);

                // Dock windows into appropriate sections
                ImGui::DockBuilderDockWindow(SceneEditorTool->GetToolName().c_str(), topDockID);
                ImGui::DockBuilderDockWindow(ConsoleLogTool->GetToolName().c_str(), bottomLeftDockID);
                ImGui::DockBuilderDockWindow(ContentBrowser->GetToolName().c_str(), bottomRightDockID);
            }

            // Create the actual dock space
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0);
            ImGui::DockSpace(DockspaceID, viewport->WorkSize, 0, &EditorWindowClass);
            ImGui::PopStyleVar();
        }

        
        ImGui::End();

        if (!FocusTargetWindowName.empty())
        {
            ImGuiWindow* Window = ImGui::FindWindowByName(FocusTargetWindowName.c_str());
            if (Window == nullptr || Window->DockNode == nullptr || Window->DockNode->TabBar == nullptr)
            {
                FocusTargetWindowName.clear();
                return;
            }

            ImGuiID TabID = 0;
            for (int i = 0; i < Window->DockNode->TabBar->Tabs.size(); ++i)
            {
                ImGuiTabItem* pTab = &Window->DockNode->TabBar->Tabs[i];
                if (pTab->Window->ID == Window->ID)
                {
                    TabID = pTab->ID;
                    break;
                }
            }

            if (TabID != 0)
            {
                Window->DockNode->TabBar->NextSelectedTabId = TabID;
                ImGui::SetWindowFocus(FocusTargetWindowName.c_str());
            }

            FocusTargetWindowName.clear();
            
        }

        if (bDearImGuiDemoWindowOpen)
        {
            ImGui::ShowDemoWindow(&bDearImGuiDemoWindowOpen);
        }

        if (bShowObjectDebug)
        {
            ImGui::SetNextWindowSize({ 700.0f, 600.0f }, ImGuiCond_FirstUseEver);
            FString Name = "CObject List - Num: " + eastl::to_string(GObjectVector.size());

            if (ImGui::Begin(Name.c_str(), &bShowObjectDebug))
            {
                THashMap<FString, TVector<CObjectBase*>> PackageToObjects;
                for (CObjectBase* Object : GObjectVector)
                {
                    FString PackageName = Object->GetPackage() ? Object->GetPackage()->GetName().ToString() : "None";
                    PackageToObjects[PackageName].push_back(Object);
                }

                for (const auto& Pair : PackageToObjects)
                {
                    const FString& PackageName = Pair.first;
                    const TVector<CObjectBase*>& Objects = Pair.second;

                    ImGuiTreeNodeFlags Flags = 0;
                    if (ImGui::TreeNodeEx(PackageName.c_str(), Flags))
                    {
                        if (ImGui::BeginTable(PackageName.c_str(), 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
                        {
                            ImGui::TableSetupColumn("Object Name");
                            ImGui::TableHeadersRow();

                            for (CObjectBase* Object : Objects)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(Object->GetName().ToString().c_str());
                            }

                            ImGui::EndTable();
                        }
                        ImGui::TreePop();
                    }
                }
            }

            ImGui::End();
        }

        FEditorTool* ToolToClose = nullptr;
        
        for (FEditorTool* Tool : EditorTools)
        {
            if (!SubmitToolMainWindow(UpdateContext, Tool, DockspaceID))
            {
                ToolToClose = Tool;
            }
        }

        for (FEditorTool* Tool : EditorTools)
        {
            if (Tool == ToolToClose)
            {
                continue;
            }
            
            DrawToolContents(UpdateContext, Tool);
        }

        if (ToolToClose)
        {
            DestroyTool(UpdateContext, ToolToClose);
        }

        while (!ToolsPendingAdd.empty())
        {
            FEditorTool* NewTool = ToolsPendingAdd.front();
            ToolsPendingAdd.pop();

            EditorTools.push_back(NewTool);
        }

        if (!FProject::Get()->HasLoadedProject() && !FileBrowser.IsOpened())
        {
            FileBrowser = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_CreateNewDir);
            FileBrowser.SetTitle("Select Project to load.");
            FileBrowser.SetTypeFilters({".lproject"});

            FileBrowser.Open();
        }
        
        FileBrowser.Display();
        if (FileBrowser.HasSelected())
        {
            std::filesystem::path Path = FileBrowser.GetSelected();
            FProject::Get()->LoadProject(Path.string().c_str());
            FileBrowser.ClearSelected();
            FileBrowser.Close();
            ContentBrowser->RefreshContentBrowser();
        }
        
        if (ModalManager.HasModal())
        {
            ModalManager.DrawDialogue(UpdateContext);
        }
    }

    void FEditorUI::OnUpdate(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE()
        for (FEditorTool* Tool : EditorTools)
        {
            if (Tool->HasScene())
            {
                Tool->SceneUpdate(UpdateContext);
            }
        }
    }

    void FEditorUI::OnEndFrame(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE()
    }
    
    void FEditorUI::DestroyTool(const FUpdateContext& UpdateContext, FEditorTool* Tool)
    {
        auto Itr = eastl::find(EditorTools.begin(), EditorTools.end(), Tool);
        Assert(Itr != EditorTools.end())

        EditorTools.erase(Itr);
        for (auto MapItr = ActiveAssetTools.begin(); MapItr != ActiveAssetTools.end(); ++MapItr)
        {
            if (MapItr->second == Tool)
            {
                ActiveAssetTools.erase(MapItr);
                break;
            }
        }
        
        Tool->Deinitialize(UpdateContext);
        Memory::Delete(Tool);
    }

    void FEditorUI::PushModal(const FString& Title, ImVec2 Size, TFunction<bool(const FUpdateContext&)> DrawFunction)
    {
        ModalManager.CreateModalDialogue(Title, Size, DrawFunction);
    }

    
    void FEditorUI::OpenAssetEditor(CObject* InAsset)
    {
        /** Temp garbage for now */
        if (InAsset != nullptr && ActiveAssetTools.find(InAsset) == ActiveAssetTools.end())
        {
            FEditorTool* NewTool = nullptr;
            if (InAsset->IsA<CMaterial>())
            {
                NewTool = CreateTool<FMaterialEditorTool>(this, InAsset);
            }
            else if (InAsset->IsA<CTexture>())
            {
                NewTool = CreateTool<FTextureEditorTool>(this, InAsset);
            }
            else if (InAsset->IsA<CStaticMesh>())
            {
                NewTool = CreateTool<FMeshEditorTool>(this, InAsset);
            }
            
            ActiveAssetTools.insert_or_assign(InAsset, NewTool);
        }
    }
    
    void FEditorUI::EditorToolLayoutCopy(FEditorTool* SourceTool)
    {
        ImGuiID sourceToolID = SourceTool->GetPrevDockspaceID();
        ImGuiID destinationToolID = SourceTool->GetCurrDockspaceID();
        Assert(sourceToolID != 0 && destinationToolID != 0)
        
        // Helper to build an array of strings pointer into the same contiguous memory buffer.
        struct ContiguousStringArrayBuilder
        {
            void AddEntry( const char* data, size_t dataLength )
            {
                const int32 bufferSize = (int32_t) m_buffer.size();
                m_offsets.push_back( bufferSize );
                const int32 offset = bufferSize;
                m_buffer.resize( bufferSize + (int32_t) dataLength );
                memcpy( m_buffer.data() + offset, data, dataLength );
            }

            void BuildPointerArray( ImVector<const char*>& outArray )
            {
                outArray.resize( (int32_t) m_offsets.size() );
                for (int32 n = 0; n < (int32) m_offsets.size(); n++)
                {
                    outArray[n] = m_buffer.data() + m_offsets[n];
                }
            }

            TVector<char>       m_buffer;
            TVector<int32>    m_offsets;
        };

        ContiguousStringArrayBuilder namePairsBuilder;

        for (FEditorTool::FToolWindow* Window : SourceTool->GetToolWindows())
        {
            const FInlineString sourceToolWindowName = FEditorTool::GetToolWindowName(Window->Name.c_str(), sourceToolID);
            const FInlineString destinationToolWindowName = FEditorTool::GetToolWindowName(Window->Name.c_str(), destinationToolID);
            namePairsBuilder.AddEntry( sourceToolWindowName.c_str(), sourceToolWindowName.length() + 1 );
            namePairsBuilder.AddEntry( destinationToolWindowName.c_str(), destinationToolWindowName.length() + 1 );
        }

        // Perform the cloning
        if (ImGui::DockContextFindNodeByID( ImGui::GetCurrentContext(), sourceToolID))
        {
            // Build the same array with char* pointers at it is the input of DockBuilderCopyDockspace() (may change its signature?)
            ImVector<const char*> windowRemapPairs;
            namePairsBuilder.BuildPointerArray(windowRemapPairs);

            ImGui::DockBuilderCopyDockSpace(sourceToolID, destinationToolID, &windowRemapPairs);
            ImGui::DockBuilderFinish(destinationToolID);
        }
    }

    bool FEditorUI::SubmitToolMainWindow(const FUpdateContext& UpdateContext, FEditorTool* EditorTool, ImGuiID TopLevelDockspaceID)
    {
        Assert(EditorTool != nullptr)
        Assert(TopLevelDockspaceID != 0)

        bool bIsToolStillOpen = true;
        bool* bIsToolOpen = (EditorTool == SceneEditorTool) ? nullptr : &bIsToolStillOpen; // Prevent closing the map-editor editor tool
        
        // Top level editors can only be docked with each others
        ImGui::SetNextWindowClass(&EditorWindowClass);
        if (EditorTool->GetDesiredDockID() != 0)
        {
            ImGui::SetNextWindowDockID(EditorTool->GetDesiredDockID());
            EditorTool->DesiredDockID = 0;
        }
        else
        {
            ImGui::SetNextWindowDockID(TopLevelDockspaceID, ImGuiCond_FirstUseEver);
        }
        
        ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

        ImGuiWindow* CurrentWindow = ImGui::FindWindowByName(EditorTool->GetToolName().c_str());
        const bool bVisible = CurrentWindow != nullptr && !CurrentWindow->Hidden;
        
        ImVec4 VisibleColor   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 NotVisibleColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, bVisible ? VisibleColor : NotVisibleColor);
        ImGui::SetNextWindowSizeConstraints(ImVec2(128, 128), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowSize(ImVec2(1024, 768), ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::Begin(EditorTool->GetToolName().c_str(), bIsToolOpen, WindowFlags);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy))
        {
            LastActiveTool = EditorTool;
        }
        
        // Set WindowClass based on per-document ID, so tabs from Document A are not dockable in Document B etc. We could be using any ID suiting us, e.g. &doc
        // We also set ParentViewportId to request the platform back-end to set parent/child relationship at the windowing level
        EditorTool->ToolWindowsClass.ClassId = EditorTool->GetID();
        EditorTool->ToolWindowsClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration;
        EditorTool->ToolWindowsClass.ParentViewportId = ImGui::GetWindowViewport()->ID;
        EditorTool->ToolWindowsClass.DockingAllowUnclassed = true;

        // Track LocationID change so we can fork/copy the layout data according to where the window is going + reference count
        // LocationID ~~ (DockId != 0 ? DockId : DocumentID) // When we are in a loose floating window we use our own document id instead of the dock id
        EditorTool->CurrDockID = ImGui::GetWindowDockID();
        EditorTool->PrevLocationID = EditorTool->CurrLocationID;
        EditorTool->CurrLocationID = EditorTool->CurrDockID != 0 ? EditorTool->CurrDockID : EditorTool->GetID();

        // Dockspace ID ~~ Hash of LocationID + DocType
        // So all editors of a same type inside a same tab-bar will share the same layout.
        // We will also use this value as a suffix to create window titles, but we could perfectly have an indirection to allocate and use nicer names for window names (e.g. 0001, 0002).
        EditorTool->PrevDockspaceID = EditorTool->CurrDockspaceID;
        EditorTool->CurrDockspaceID = EditorTool->CalculateDockspaceID();
        Assert(EditorTool->CurrDockspaceID != 0)
        

        ImGui::End();

        return bIsToolStillOpen;
    }

    void FEditorUI::DrawToolContents(const FUpdateContext& UpdateContext, FEditorTool* Tool)
    {
        // This is the second Begin(), as MyEditor_UpdateDocLocationAndLayout() has already done one
        // (Therefore only the p_open and flags of the first call to Begin() applies)
        ImGui::Begin(Tool->GetToolName().c_str());
        
        Assert(ImGui::GetCurrentWindow()->BeginCount == 2)
        
        const ImGuiID dockspaceID = Tool->GetCurrentDockspaceID();
        const ImVec2 DockspaceSize = ImGui::GetContentRegionAvail();

        if (Tool->PrevLocationID != 0 && Tool->PrevLocationID != Tool->CurrLocationID)
        {
            int PrevDockspaceRefCount = 0;
            int CurrDockspaceRefCount = 0;
            for (FEditorTool* OtherTool : EditorTools)
            {
                if (OtherTool->CurrDockspaceID == Tool->PrevDockspaceID)
                {
                    PrevDockspaceRefCount++;
                }
                else if (OtherTool->CurrDockspaceID == Tool->CurrDockspaceID)
                {
                    CurrDockspaceRefCount++;
                }
            }

            // Fork or overwrite settings
            // FIXME: should be able to do a "move window but keep layout" if CurrDockspaceRefCount > 1.
            // FIXME: when moving, delete settings of old windows
            EditorToolLayoutCopy(Tool);

            if (PrevDockspaceRefCount == 0)
            {
                ImGui::DockBuilderRemoveNode(Tool->PrevDockspaceID);

                // Delete settings of old windows
                // Rely on window name to ditch their .ini settings forever..
                char windowSuffix[16];
                ImFormatString(windowSuffix, IM_ARRAYSIZE(windowSuffix), "##%08X", Tool->PrevDockspaceID);
                size_t windowSuffixLength = strlen( windowSuffix );
                ImGuiContext& g = *GImGui;
                for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != nullptr; settings = g.SettingsWindows.next_chunk(settings))
                {
                    if ( settings->ID == 0 )
                    {
                        continue;
                    }
                    
                    char const* pWindowName = settings->GetName();
                    size_t windowNameLength = strlen( pWindowName );
                    if ( windowNameLength >= windowSuffixLength )
                    {
                        if (strcmp(pWindowName + windowNameLength - windowSuffixLength, windowSuffix) == 0) // Compare suffix
                        {
                            ImGui::ClearWindowSettings( pWindowName );
                        }
                    }
                }
            }
        }
        else if (ImGui::DockBuilderGetNode(Tool->GetCurrentDockspaceID()) == nullptr)
        {
            ImVec2 dockspaceSize = ImGui::GetContentRegionAvail();
            dockspaceSize.x = eastl::max(dockspaceSize.x, 1.0f);
            dockspaceSize.y = eastl::max(dockspaceSize.y, 1.0f);

            ImGui::DockBuilderAddNode(Tool->GetCurrentDockspaceID(), ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(Tool->GetCurrentDockspaceID(), dockspaceSize);
            if (!Tool->IsSingleWindowTool())
            {
                Tool->InitializeDockingLayout(Tool->GetCurrentDockspaceID(), dockspaceSize);
            }
            ImGui::DockBuilderFinish(Tool->GetCurrentDockspaceID());
        }

        // FIXME-DOCK: This is a little tricky to explain but we currently need this to use the pattern of sharing a same dockspace between tabs of a same tab bar
        bool bVisible = true;
        if (ImGui::GetCurrentWindow()->Hidden)
        {
            bVisible = false;
        }
        
        const bool bIsLastFocusedTool = (LastActiveTool == Tool);
        
        Tool->Update(UpdateContext);
        Tool->bViewportFocused = false;
        Tool->bViewportHovered = false;

        if (Tool->HasScene())
        {
            Tool->GetScene()->SetPaused(!bVisible);
        }
        
        if (!bVisible)
        {
            if (!Tool->IsSingleWindowTool())
            {
                // Keep alive document dockspace so windows that are docked into it but which visibility are not linked to the dockspace visibility won't get undocked.
                ImGui::DockSpace(dockspaceID, DockspaceSize, ImGuiDockNodeFlags_KeepAliveOnly, &Tool->ToolWindowsClass);
            }
            
            ImGui::End();
            
            return;
        }

        
        if (Tool->HasFlag(EEditorToolFlags::Tool_WantsToolbar))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 16));
            if (ImGui::BeginMenuBar())
            {
                Tool->DrawMainToolbar(UpdateContext);
                ImGui::EndMenuBar();
            }
            ImGui::PopStyleVar();
        }

        if (Tool->IsSingleWindowTool())
        {
            Assert(Tool->ToolWindows.size() == 1);
            Tool->ToolWindows[0]->DrawFunction(UpdateContext, bIsLastFocusedTool);
        }
        else
        {
            ImGui::DockSpace(dockspaceID, DockspaceSize, ImGuiDockNodeFlags_None, &Tool->ToolWindowsClass);
        }
    
        ImGui::End();


        if (!Tool->IsSingleWindowTool())
        {
            for (FEditorTool::FToolWindow* Window : Tool->ToolWindows)
            {
                const FInlineString ToolWindowName = FEditorTool::GetToolWindowName(Window->Name.c_str(), Tool->GetCurrentDockspaceID());

                // When multiple documents are open, floating tools only appear for focused one
                if (!bIsLastFocusedTool)
                {
                    if (ImGuiWindow* pWindow = ImGui::FindWindowByName(ToolWindowName.c_str()))
                    {
                        ImGuiDockNode* pWindowDockNode = pWindow->DockNode;
                        if (pWindowDockNode == nullptr && pWindow->DockId != 0)
                        {
                            pWindowDockNode = ImGui::DockContextFindNodeByID(ImGui::GetCurrentContext(), pWindow->DockId);
                        }
                       
                        if (pWindowDockNode == nullptr || ImGui::DockNodeGetRootNode(pWindowDockNode)->ID != dockspaceID)
                        {
                            continue;
                        }
                    }
                }
            
                if (Window->bViewport)
                {
                    constexpr ImGuiWindowFlags viewportWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus;
                    ImGui::SetNextWindowClass(&Tool->ToolWindowsClass);

                    //-- Setup viewport for scene.
                    
                    ImGui::SetNextWindowSizeConstraints(ImVec2(128, 128), ImVec2(FLT_MAX, FLT_MAX));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    bool const DrawViewportWindow = ImGui::Begin(ToolWindowName.c_str(), nullptr, viewportWindowFlags);
                    ImGui::PopStyleVar();
                
                    if (DrawViewportWindow)
                    {
                        FScene* Scene = Tool->GetScene();
                        FSceneRenderer* SceneRenderer = UpdateContext.GetSubsystem<FSceneManager>()->GetSceneRendererForScene(Scene);
                        
                        FRHIImageRef RenderTarget = SceneRenderer->GetRenderTarget();

                        FRenderManager* RenderManager = UpdateContext.GetSubsystem<FRenderManager>();
                        IImGuiRenderer* ImGuiRenderer = RenderManager->GetImGuiRenderer();
                        
                        ImTextureID ViewportTexture = ImGuiRenderer->GetOrCreateImTexture(RenderTarget);
                        
                        Tool->bViewportFocused = ImGui::IsWindowFocused();
                        Tool->bViewportHovered = ImGui::IsWindowHovered();
                        Tool->DrawViewport(UpdateContext, ViewportTexture);
                        
                    }
                    
                    ImGui::End();
                }
                else
                {
                    ImGuiWindowFlags ToolWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus;

                    ImGui::SetNextWindowClass(&Tool->ToolWindowsClass);

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImGui::GetStyle().WindowPadding);
                    bool const drawToolWindow = ImGui::Begin(ToolWindowName.c_str(), &Window->bOpen, ToolWindowFlags);
                    ImGui::PopStyleVar();

                    if (drawToolWindow)
                    {
                        bool const isToolWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy);
                        Window->DrawFunction(UpdateContext, isToolWindowFocused);
                    }
                    
                    ImGui::End();
                }
                
            }
        }

        if (Tool->HasScene())
        {
            Tool->SetEditorCameraEnabled(Tool->bViewportFocused);
        }
        
    }

    void FEditorUI::CreateGameViewportTool(const FUpdateContext& UpdateContext)
    {
    }

    void FEditorUI::DestroyGameViewportTool(const FUpdateContext& UpdateContext)
    {
        
    }

    void FEditorUI::HandleUserInput(const FUpdateContext& UpdateContext)
    {
        
    }


    void FEditorUI::DrawTitleBarMenu(const FUpdateContext& UpdateContext)
    {
        ImGui::Text(LE_ICON_GAVEL);
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(175, 1), ImVec2(200, 1000));

        
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Save", nullptr);
            
            if (ImGui::MenuItem("Recompile Shaders", nullptr))
            {
                UpdateContext.GetSubsystem<FRenderManager>()->GetRenderContext()->CompileEngineShaders();    
            }
            ImGui::EndMenu();
        }

        
        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Project Settings", nullptr))
            {
                ModalManager.CreateModalDialogue("Project Settings", ImVec2(800, 800), [this] (const FUpdateContext& Ctx) -> bool
                {
                    if (ImGui::Button("Close"))
                    {
                        return true;
                    }

                    return false;
                    
                });
            }

            if (ImGui::MenuItem("Asset Registry", nullptr))
            {
                ModalManager.CreateModalDialogue("Asset Registry", ImVec2(1400, 800), [this] (const FUpdateContext& Ctx) -> bool
                {
                    if (ImGui::Button("Close"))
                    {
                        return true;
                    }
                    
                    return false;
                });
            }
            
            if (ImGui::MenuItem("Open Project", nullptr))
            {
                ModalManager.CreateModalDialogue("Open Project", ImVec2(800, 800), [this] (const FUpdateContext& Ctx) -> bool
                {
                    if (ImGui::Button("Close"))
                    {
                        return true;
                    }
                
                    return false;
                                    
                });
            }
            
            if (ImGui::MenuItem("New Project", nullptr))
            {
                ModalManager.CreateModalDialogue("New Project", ImVec2(800, 800), [this] (const FUpdateContext& Ctx) -> bool
                {
                    if (ImGui::Button("Close"))
                    {
                        return true;
                    }
                
                    return false;
                                    
                });
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("ImGui Demo Window", nullptr, &bDearImGuiDemoWindowOpen, !bDearImGuiDemoWindowOpen);

            bool bVSyncEnabled = UpdateContext.GetSubsystem<FRenderManager>()->GetRenderContext()->IsVSyncEnabled();
            if (ImGui::MenuItem("Enable V-Sync", nullptr, bVSyncEnabled))
            {
                UpdateContext.GetSubsystem<FRenderManager>()->GetRenderContext()->SetVSyncEnabled(!bVSyncEnabled);
            }
            ImGui::MenuItem("CObject List", nullptr, &bShowObjectDebug, !bShowObjectDebug);
            if (ImGui::MenuItem("Profiler", nullptr))
            {
                FString LuminaDirEnv = std::getenv("LUMINA_DIR");
                FString FullPath = LuminaDirEnv + "/External/Tracy/tracy-profiler.exe";
                FString FullCommand = "start " + FullPath;
                system(FullCommand.c_str());
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }

        
        ImGui::PopStyleColor(2);
    }
    
    void FEditorUI::DrawTitleBarInfoStats(const FUpdateContext& UpdateContext)
    {
        ImGui::SameLine();
        float const currentFPS = 1.0f / (float)UpdateContext.GetDeltaTime();
        TInlineString<100> const perfStats( TInlineString<100>::CtorSprintf(),  "FPS: %3.0f", currentFPS );
        ImGui::Text(perfStats.c_str());

        
        ImGui::SameLine();
        SIZE_T CObjectCount = GObjectVector.size();
        TInlineString<100> const ObjectStats(TInlineString<100>::CtorSprintf(),  "CObject Count: %i", CObjectCount);
        ImGui::Text(ObjectStats.c_str());

        
        ImGui::SameLine();
        float const allocatedMemory = (float)Memory::GetTotalRequestedMemory() / 1024.0f / 1024.0f;
        TInlineString<100> const memStats( TInlineString<100>::CtorSprintf(), "MEM: %.2fMB", allocatedMemory );
        ImGui::Text(memStats.c_str());
    }
}
