#pragma once
#include "UI/Tools/AssetEditors/AssetEditorTool.h"

namespace Lumina
{
    class FArchetypeEditorTool : public FAssetEditorTool
    {
    public:
        
        LUMINA_EDITOR_TOOL(FArchetypeEditorTool)
        
        FArchetypeEditorTool(IEditorToolContext* Context, CObject* InAsset);


        bool IsSingleWindowTool() const override { return false; }
        const char* GetTitlebarIcon() const override { return LE_ICON_FORMAT_LIST_BULLETED_TYPE; }
        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;
        void OnAssetLoadFinished() override;
        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;

        ImGuizmo::OPERATION GuizmoOp = ImGuizmo::TRANSLATE;
    };
}
