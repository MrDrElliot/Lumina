#pragma once
#include "Memory/RefCounted.h"
#include "Scene/Scene.h"
#include "Tools/EditorToolContext.h"
#include "Tools/UI/DevelopmentToolUI.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    class FConsoleLogEditorTool;
}

namespace Lumina
{
    class FEntitySceneEditorTool;
    class FSceneManager;
    class FEditorTool;
}


namespace Lumina
{
    class FEditorUI : public IDevelopmentToolUI, public IEditorToolContext
    {
    public:
        
        FEditorUI();
        ~FEditorUI() override;

        void Initialize(const FUpdateContext& UpdateContext) override;
        void Deinitialize(const FUpdateContext& UpdateContext) override;

        void OnStartFrame(const FUpdateContext& UpdateContext) override;
        void OnUpdate(const FUpdateContext& UpdateContext) override;
        void OnEndFrame(const FUpdateContext& UpdateContext) override;

        void DestroyTool(const FUpdateContext& UpdateContext, FEditorTool* Tool);

    
    private:

        void EditorToolLayoutCopy(FEditorTool* SourceTool);

        void SubmitToolMainWindow(const FUpdateContext& UpdateContet, FEditorTool* EditorTool, ImGuiID TopLevelDockspaceID);
        void DrawToolContents(const FUpdateContext& UpdateContext, FEditorTool* Tool);
        

        void CreateGameViewportTool(const FUpdateContext& UpdateContext);
        void DestroyGameViewportTool(const FUpdateContext& UpdateContext);

        
        void DrawTitleBarMenu(const FUpdateContext& UpdateContext);
        void DrawTitleBarInfoStats(const FUpdateContext& UpdateContext);

        
        void HandleUserInput(const FUpdateContext& UpdateContext);


    private:

        ImGuiX::ApplicationTitleBar                     TitleBar;
        ImGuiWindowClass                                EditorWindowClass;

        FSceneManager*                                  SceneManager = nullptr;
        FEntitySceneEditorTool*                         SceneEditorTool = nullptr;

        FConsoleLogEditorTool*                          ConsoleLogTool = nullptr;
        TVector<FEditorTool*>                           EditorTools;
        FEditorTool*                                    LastActiveTool = nullptr;
        bool                                            bDearImGuiDemoWindowOpen = false;
        FString                                         FocusTargetWindowName; // If this is set we need to switch focus to this window

    };
    
}
