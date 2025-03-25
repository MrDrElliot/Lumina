#pragma once

#include "UI/Tools/AssetEditors/AssetEditorTool.h"

namespace Lumina
{
    class FMaterialNodeGraph;

    class FMaterialEditorTool : public FAssetEditorTool
    {
    public:

        struct FCompilationResultInfo
        {
            FString CompilationLog;
            bool bIsError;
        };

        LUMINA_EDITOR_TOOL(FMaterialEditorTool)

        FMaterialEditorTool(IEditorToolContext* Context, const FAssetPath& Path)
            : FAssetEditorTool(Context, "Material", Path)
        {
        }

        bool IsSingleWindowTool() const override { return false; }
        const char* GetTitlebarIcon() const override { return LE_ICON_FORMAT_LIST_BULLETED_TYPE; }
        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;

        void OnAssetLoadFinished() override;
        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void DrawMaterialGraph(const FUpdateContext& UpdateContext);
        void DrawMaterialProperties(const FUpdateContext& UpdateContext);
        void DrawMaterialPreview(const FUpdateContext& UpdateContext);
        void DrawCompilationLog(const FUpdateContext& UpdateContext);

        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;

    private:

        FCompilationResultInfo CompilationResult;
        FMaterialNodeGraph* NodeGraph;
        
    };
}
