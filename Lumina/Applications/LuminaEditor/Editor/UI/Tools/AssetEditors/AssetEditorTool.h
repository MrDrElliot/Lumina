#pragma once

#include "UI/Tools/EditorTool.h"

namespace Lumina
{
    class FAssetEditorTool : public FEditorTool
    {
    public:
        
        FAssetEditorTool(IEditorToolContext* Context, const FString& AssetName, const FAssetPath& Asset)
            : FEditorTool(Context, AssetName, nullptr)
        {
        }

    protected:

        TRefCountPtr<FAssetRecord>      Asset;

    };
}
