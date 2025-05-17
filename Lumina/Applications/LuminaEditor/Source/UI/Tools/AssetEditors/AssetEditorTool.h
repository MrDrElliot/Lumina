#pragma once

#include "Assets/AssetManager/AssetManager.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectPtr.h"
#include "UI/Tools/EditorTool.h"

namespace Lumina
{
    class FAssetEditorTool : public FEditorTool
    {
    public:
        
        FAssetEditorTool(IEditorToolContext* Context, const FString& AssetName, CObject* InAsset)
            : FEditorTool(Context, AssetName, nullptr)
            , bAssetLoadBroadcasted(false)
        {
            Asset = InAsset;
        }

        void Update(const FUpdateContext& UpdateContext) override
        {
            if (!bAssetLoadBroadcasted && Asset != nullptr)
            {
                OnAssetLoadFinished();
                bAssetLoadBroadcasted = true;
            }
        }
        
        virtual void OnAssetLoadFinished() = 0;

    protected:

        TObjectPtr<CObject>         Asset;
        uint8                       bAssetLoadBroadcasted:1;

    };
}
