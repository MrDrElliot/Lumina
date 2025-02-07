#pragma once
#include "EditorTool.h"

namespace Lumina
{
    /**
     * Base class for display and manipulating scenes.
     */
    class FSceneEditorTool : public FEditorTool
    {

        LUMINA_SINGLETON_EDITOR_TOOL(FSceneEditorTool)
        
    public:
        
        FSceneEditorTool(const IEditorToolContext* Context, const TRefPtr<FScene>& Scene);

        void OnInitialize(const FUpdateContext& UpdateContext) override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override { }

        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;
        
    protected:

        void DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused);
    
    };
}