#pragma once
#include "EditorTool.h"

namespace Lumina
{
    class FGamePreviewTool : public FEditorTool
    {
    public:

        LUMINA_EDITOR_TOOL(FGamePreviewTool)
        
        FGamePreviewTool(IEditorToolContext* Context, CWorld* InWorld);


        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;
        const char* GetTitlebarIcon() const override { return LE_ICON_EARTH; }
        void Update(const FUpdateContext& UpdateContext) override;

        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;
        
        void DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureRef ViewportTexture, ImVec2 ViewportSize) override;

    private:

        
    
    };
}
