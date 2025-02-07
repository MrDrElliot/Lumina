#pragma once
#include "SceneEditorTool.h"

namespace Lumina
{
    class FEntitySceneEditorTool : public FSceneEditorTool
    {
    public:
        
        FEntitySceneEditorTool(const IEditorToolContext* Context, const TRefPtr<FScene>& Scene);
        
        const char* GetTitlebarIcon() const override { return LE_ICON_EARTH; }
    };
}
