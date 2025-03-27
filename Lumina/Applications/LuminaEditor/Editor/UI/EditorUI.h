#pragma once
#include "Memory/RefCounted.h"
#include "Scene/Scene.h"
#include "Tools/EditorToolContext.h"
#include "Tools/EditorToolModal.h"
#include "Tools/UI/DevelopmentToolUI.h"
#include "Tools/UI/ImGui/imfilebrowser.h"
#include "Tools/UI/ImGui/ImGuiX.h"



namespace Lumina
{
    class FEditorToolModal;
    class FContentBrowserEditorTool;
    class FRendererInfoEditorTool;
    class FRendererInfoEditorTool;
    class FPrimitiveDrawManager;
    class FConsoleLogEditorTool;
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

        void OpenAssetPath(const FAssetPath& InPath) override;
        
        template<typename T, typename... Args>
        requires std::is_base_of_v<FEditorTool, T>
        T* CreateTool(Args&&... args)
        {
            T* NewTool = FMemory::New<T>(TForward<Args>(args)...);
            NewTool->Initialize();
            EditorTools.emplace_back(NewTool);
            return NewTool;
        }
        
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

        ImGui::FileBrowser                              FileBrowser;
        ImGuiX::ApplicationTitleBar                     TitleBar;
        ImGuiWindowClass                                EditorWindowClass;

        FSceneManager*                                  SceneManager = nullptr;
        FEntitySceneEditorTool*                         SceneEditorTool = nullptr;

        FConsoleLogEditorTool*                          ConsoleLogTool = nullptr;
        FContentBrowserEditorTool*                      ContentBrowser = nullptr;
        TVector<FEditorTool*>                           EditorTools;
        FEditorTool*                                    LastActiveTool = nullptr;
        FString                                         FocusTargetWindowName; // If this is set we need to switch focus to this window

        THashMap<FGuid, FEditorTool*>                   ActiveAssetTools;
        
        FEditorModalManager                             ModalManager; 
        bool                                            bDearImGuiDemoWindowOpen = false;

    };

}
