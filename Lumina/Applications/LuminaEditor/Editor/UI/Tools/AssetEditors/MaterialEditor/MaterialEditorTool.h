#pragma once

#include "UI/Tools/AssetEditors/AssetEditorTool.h"

namespace Lumina
{
    class FMaterialEditorTool : public FAssetEditorTool
    {
    public:

        LUMINA_EDITOR_TOOL(FMaterialEditorTool)

        FMaterialEditorTool(IEditorToolContext* Context, const FAssetPath& Path)
            : FAssetEditorTool(Context, "Material", Path)
        {
        }

        bool IsSingleWindowTool() const override { return false; }
        const char* GetTitlebarIcon() const override { return LE_ICON_FORMAT_LIST_BULLETED_TYPE; }
        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;

        void Update(const FUpdateContext& UpdateContext) override;

        void DrawMaterialGraph(const FUpdateContext& UpdateContext);
        void DrawMaterialProperties(const FUpdateContext& UpdateContext);
        void DrawMaterialPreview(const FUpdateContext& UpdateContext);

        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;

    private:
        
    };
}
