#pragma once
#include "Memory/RefCounted.h"
#include "Scene/Scene.h"
#include "Tools/EditorToolContext.h"
#include "Tools/EditorToolModal.h"
#include "Tools/UI/DevelopmentToolUI.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    class FEditorToolModal;
    class FContentBrowserEditorTool;
}

namespace Lumina
{
    class FPrimitiveDrawManager;
}

namespace Lumina
{
    class FRendererInfoEditorTool;
}

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

        void PushModal(const FString& Title, ImVec2 Size, TFunction<bool(const FUpdateContext&)> DrawFunction) override;
        
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
        FContentBrowserEditorTool*                      ContentBrowser = nullptr;
        TVector<FEditorTool*>                           EditorTools;
        FEditorTool*                                    LastActiveTool = nullptr;
        FString                                         FocusTargetWindowName; // If this is set we need to switch focus to this window

        FEditorModalManager                             ModalManager; 
        bool                                            bDearImGuiDemoWindowOpen = false;

    };
    
}
