#pragma once
#include "SceneEditorTool.h"

namespace Lumina
{
    class FEntitySceneEditorTool : public FSceneEditorTool
    {
    public:
        
        FEntitySceneEditorTool(const IEditorToolContext* Context, FScene* InScene);
        
        const char* GetTitlebarIcon() const override { return LE_ICON_EARTH; }
    };
}
