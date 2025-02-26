#include "EditorUI.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "Memory/Memory.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene/Entity/Systems/DebugCameraEntitySystem.h"
#include "Tools/ConsoleLogEditorTool.h"
#include "Tools/EditorTool.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "Tools/EntitySceneEditorTool.h"
#include "Tools/RendererInfoEditorTool.h"
#include "Tools/UI/ImGui/Vulkan/VulkanImGuiRender.h"

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
        EditorWindowClass.ClassId = ImHashStr("EditorWindowClass");
        EditorWindowClass.DockingAllowUnclassed = false;
        EditorWindowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
        EditorWindowClass.ViewportFlagsOverrideClear = ImGuiViewportFlags_NoDecoration | ImGuiViewportFlags_NoTaskBarIcon;
        EditorWindowClass.ParentViewportId = 0; // Top level window
        EditorWindowClass.DockingAllowUnclassed = false;
        EditorWindowClass.DockingAlwaysTabBar = true;


        SubsystemManager = UpdateContext.GetSubsystemManager();
        SceneManager = UpdateContext.GetSubsystem<FSceneManager>();

        
        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
        NewScene->Initialize(UpdateContext);
        NewScene->RegisterSystem(FMemory::New<FDebugCameraEntitySystem>());
        
        
        SceneEditorTool = FMemory::New<FEntitySceneEditorTool>(this, NewScene);
        SceneEditorTool->Initialize(UpdateContext);
        EditorTools.emplace_back(SceneEditorTool);

        
        ConsoleLogTool = FMemory::New<FConsoleLogEditorTool>(this);
        ConsoleLogTool->Initialize(UpdateContext);
        EditorTools.emplace_back(ConsoleLogTool);

        
        RendererInfo = FMemory::New<FRendererInfoEditorTool>(this);
        RendererInfo->Initialize(UpdateContext);
        EditorTools.emplace_back(RendererInfo);

        
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
        RendererInfo = nullptr;
    }

    void FEditorUI::OnStartFrame(const FUpdateContext& UpdateContext)
    {
        Assert(UpdateContext.GetUpdateStage() == EUpdateStage::FrameStart);

        auto TitleBarLeftContents = [this, &UpdateContext] ()
        {
            DrawTitleBarMenu(UpdateContext);
        };

        auto TitleBarRightContents = [this, &UpdateContext] ()
        {
            DrawTitleBarInfoStats(UpdateContext);
        };

        TitleBar.Draw(TitleBarLeftContents, 250, TitleBarRightContents, 160);

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
                ImGui::DockBuilderDockWindow(RendererInfo->GetToolName().c_str(), bottomRightDockID);
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

        for (FEditorTool* Tool : EditorTools)
        {
            SubmitToolMainWindow(UpdateContext, Tool, DockspaceID);
        }

        for (FEditorTool* Tool : EditorTools)
        {
            DrawToolContents(UpdateContext, Tool);
        }
        
        
    }

    void FEditorUI::OnUpdate(const FUpdateContext& UpdateContext)
    {
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

    }
    
    void FEditorUI::DestroyTool(const FUpdateContext& UpdateContext, FEditorTool* Tool)
    {
        auto Itr = eastl::find(EditorTools.begin(), EditorTools.end(), Tool);
        Assert(Itr != EditorTools.end());

        
        Tool->Deinitialize(UpdateContext);
        FMemory::Delete(Tool);
        EditorTools.erase(Itr);
    }

    void FEditorUI::EditorToolLayoutCopy(FEditorTool* SourceTool)
    {
        ImGuiID sourceToolID = SourceTool->PrevDockspaceID;
        ImGuiID destinationToolID = SourceTool->CurrDockspaceID;
        Assert(sourceToolID != 0 && destinationToolID != 0);
        
        // Helper to build an array of strings pointer into the same contiguous memory buffer.
        struct ContiguousStringArrayBuilder
        {
            void AddEntry( const char* data, size_t dataLength )
            {
                int32_t const bufferSize = (int32_t) m_buffer.size();
                m_offsets.push_back( bufferSize );
                int32_t const offset = bufferSize;
                m_buffer.resize( bufferSize + (int32_t) dataLength );
                memcpy( m_buffer.data() + offset, data, dataLength );
            }

            void BuildPointerArray( ImVector<const char*>& outArray )
            {
                outArray.resize( (int32_t) m_offsets.size() );
                for ( int32_t n = 0; n < (int32_t) m_offsets.size(); n++ )
                {
                    outArray[n] = m_buffer.data() + m_offsets[n];
                }
            }

            TVector<char>       m_buffer;
            TVector<int32_t>    m_offsets;
        };

        ContiguousStringArrayBuilder namePairsBuilder;

        for (FEditorTool::FToolWindow* Window : SourceTool->ToolWindows)
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

    void FEditorUI::SubmitToolMainWindow(const FUpdateContext& UpdateContet, FEditorTool* EditorTool, ImGuiID TopLevelDockspaceID)
    {
        Assert(EditorTool != nullptr);
        Assert(TopLevelDockspaceID != 0);

        bool isToolStillOpen = true;
        bool* pIsToolOpen = ( EditorTool == SceneEditorTool ) ? nullptr : &isToolStillOpen; // Prevent closing the map-editor editor tool
        
        // Top level editors can only be docked with each others
        ImGui::SetNextWindowClass(&EditorWindowClass);
        if (EditorTool->DesiredDockID != 0)
        {
            ImGui::SetNextWindowDockID(EditorTool->DesiredDockID);
            EditorTool->DesiredDockID = 0;
        }
        else
        {
            ImGui::SetNextWindowDockID(TopLevelDockspaceID, ImGuiCond_FirstUseEver);
        }
        
        ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
        
       // ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
        ImGui::SetNextWindowSizeConstraints(ImVec2(128, 128), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowSize(ImVec2(1024, 768), ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::Begin(EditorTool->GetToolName().c_str(), pIsToolOpen, WindowFlags);
        ImGui::PopStyleVar();
      //  ImGui::PopStyleColor();

        ImGuiWindow* pCurrentWindow = ImGui::FindWindowByName(EditorTool->GetToolName().c_str());

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy))
        {
            LastActiveTool = EditorTool;
        }
        
        // Set WindowClass based on per-document ID, so tabs from Document A are not dockable in Document B etc. We could be using any ID suiting us, e.g. &doc
        // We also set ParentViewportId to request the platform back-end to set parent/child relationship at the windowing level
        EditorTool->ToolWindowsClass.ClassId = EditorTool->ID;
        EditorTool->ToolWindowsClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration;
        EditorTool->ToolWindowsClass.ParentViewportId = ImGui::GetWindowViewport()->ID;
        EditorTool->ToolWindowsClass.DockingAllowUnclassed = true;

        // Track LocationID change so we can fork/copy the layout data according to where the window is going + reference count
        // LocationID ~~ (DockId != 0 ? DockId : DocumentID) // When we are in a loose floating window we use our own document id instead of the dock id
        EditorTool->CurrDockID = ImGui::GetWindowDockID();
        EditorTool->PrevLocationID = EditorTool->CurrLocationID;
        EditorTool->CurrLocationID = EditorTool->CurrDockID != 0 ? EditorTool->CurrDockID : EditorTool->ID;

        // Dockspace ID ~~ Hash of LocationID + DocType
        // So all editors of a same type inside a same tab-bar will share the same layout.
        // We will also use this value as a suffix to create window titles, but we could perfectly have an indirection to allocate and use nicer names for window names (e.g. 0001, 0002).
        EditorTool->PrevDockspaceID = EditorTool->CurrDockspaceID;
        EditorTool->CurrDockspaceID = EditorTool->CalculateDockspaceID();
        Assert(EditorTool->CurrDockspaceID != 0);
        

        ImGui::End();

    }

    void FEditorUI::DrawToolContents(const FUpdateContext& UpdateContext, FEditorTool* Tool)
    {
        // This is the second Begin(), as MyEditor_UpdateDocLocationAndLayout() has already done one
        // (Therefore only the p_open and flags of the first call to Begin() applies)
        ImGui::Begin(Tool->ToolName.c_str());
        
        Assert(ImGui::GetCurrentWindow()->BeginCount == 2);
        
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
                ImFormatString( windowSuffix, IM_ARRAYSIZE( windowSuffix ), "##%08X", Tool->PrevDockspaceID);
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
                    
                    FScene* Scene = Tool->GetScene();
                    Assert(Scene != nullptr);
                    
                    FSceneRenderer* SceneRenderer = UpdateContext.GetSubsystem<FSceneManager>()->GetSceneRendererForScene(Scene);
                    Assert(SceneRenderer != nullptr);
                    

                    FVulkanImGuiRender* ImGuiRenderer = UpdateContext.GetSubsystem<FVulkanImGuiRender>();

                    
                    ImGui::SetNextWindowSizeConstraints(ImVec2(128, 128), ImVec2(FLT_MAX, FLT_MAX));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    bool const DrawViewportWindow = ImGui::Begin(ToolWindowName.c_str(), nullptr, viewportWindowFlags);
                    ImGui::PopStyleVar();
                
                    if (DrawViewportWindow)
                    {
                        Tool->bViewportFocused = ImGui::IsWindowFocused();
                        Tool->bViewportHovered = ImGui::IsWindowHovered();
                        Tool->DrawViewport(UpdateContext, nullptr);
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
        ImGui::Text("Lumina");

        ImGui::SameLine();

        if (ImGui::BeginMenu("Editor"))
        {
            ImGui::MenuItem( "ImGui Demo Window", nullptr, &bDearImGuiDemoWindowOpen, !bDearImGuiDemoWindowOpen);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("System"))
        {
            ImGui::MenuItem( "RHI Info", nullptr, &bRHIInfoOpen, !bRHIInfoOpen);
            ImGui::EndMenu();
        }
    }
    
    void FEditorUI::DrawTitleBarInfoStats(const FUpdateContext& UpdateContext)
    {
        ImGui::SameLine();
        float const currentFPS = 1.0f / UpdateContext.GetDeltaTime();
        TInlineString<100> const perfStats( TInlineString<100>::CtorSprintf(),  "FPS: %3.0f", currentFPS );
        ImGui::Text(perfStats.c_str());

        ImGui::SameLine();
        float const allocatedMemory = FMemory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
        TInlineString<100> const memStats( TInlineString<100>::CtorSprintf(), "MEM: %.2fMB", allocatedMemory );
        ImGui::Text(memStats.c_str());
    }
}
