#pragma once
#include "EditorTool.h"

namespace Lumina
{
    class FRendererInfoEditorTool : public FEditorTool
    {

        LUMINA_SINGLETON_EDITOR_TOOL(FRendererInfoEditorTool)
        
    public:
        FRendererInfoEditorTool(const IEditorToolContext* Context)
            : FEditorTool(Context, "RHI Info", nullptr)
        {
        }

        bool IsSingleWindowTool() const override { return true; }
        const char* GetTitlebarIcon() const override { return LE_ICON_FORMAT_LIST_BULLETED_TYPE; }
        void OnInitialize(const FUpdateContext& UpdateContext) override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;
        
        
    };
}
