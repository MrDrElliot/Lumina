#pragma once

#include "Memory/RefCounted.h"
#include "Tools/EditorToolContext.h"
#include "Tools/EditorToolModal.h"
#include <ThirdParty/imgui/imgui.h>
#include "Tools/UI/DevelopmentToolUI.h"
#include "Tools/UI/ImGui/imfilebrowser.h"
#include "Tools/UI/ImGui/ImGuiX.h"


namespace Lumina
{
    class FGamePreviewTool;
}

namespace Lumina
{
    class FEditorToolModal;
    class FContentBrowserEditorTool;
    class FRendererInfoEditorTool;
    class FPrimitiveDrawManager;
    class FConsoleLogEditorTool;
    class FWorldEditorTool;
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

        void OpenAssetEditor(CObject* InAsset) override;
        void OnDestroyAsset(CObject* InAsset) override;
        
        template<typename T, typename... Args>
        requires std::is_base_of_v<FEditorTool, T> && std::constructible_from<T, Args...>
        T* CreateTool(Args&&... args);
        
    private:

        void EditorToolLayoutCopy(FEditorTool* SourceTool);

        /** Returns false if the tool wants to close */
        bool SubmitToolMainWindow(const FUpdateContext& UpdateContext, FEditorTool* EditorTool, ImGuiID TopLevelDockspaceID);
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

        FGamePreviewTool*                               GamePreviewTool = nullptr;
        FWorldEditorTool*                               WorldEditorTool = nullptr;

        FConsoleLogEditorTool*                          ConsoleLogTool = nullptr;
        FContentBrowserEditorTool*                      ContentBrowser = nullptr;
        TVector<FEditorTool*>                           EditorTools;
        FEditorTool*                                    LastActiveTool = nullptr;
        FString                                         FocusTargetWindowName; // If this is set we need to switch focus to this window

        THashMap<CObject*, FEditorTool*>                ActiveAssetTools;
        
        FEditorModalManager                             ModalManager; 
        bool                                            bDearImGuiDemoWindowOpen = false;
        bool                                            bShowObjectDebug = false;
        bool                                            bShowRenderDebug = false;
        bool                                            bShowMemoryDebug = false;
        bool                                            bShowAssetRegistry = false;

        TQueue<FEditorTool*>                            ToolsPendingAdd;
        TQueue<FEditorTool*>                            ToolsPendingDestroy;
        
    };

    template <typename T, typename ... Args>
    requires std::is_base_of_v<FEditorTool, T> && std::constructible_from<T, Args...>
    T* FEditorUI::CreateTool(Args&&... args)
    {
        T* NewTool = Memory::New<T>(std::forward<Args>(args)...);
        NewTool->Initialize();
        ToolsPendingAdd.push(NewTool);
        return NewTool;
    }
}
