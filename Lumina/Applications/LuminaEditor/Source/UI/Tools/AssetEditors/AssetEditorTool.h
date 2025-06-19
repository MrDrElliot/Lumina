#pragma once

#include "Assets/AssetManager/AssetManager.h"
#include "Core/Object/Object.h"
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

        void Deinitialize(const FUpdateContext& UpdateContext) override;

        void Update(const FUpdateContext& UpdateContext) override
        {
            if (!bAssetLoadBroadcasted && Asset != nullptr)
            {
                OnAssetLoadFinished();
                bAssetLoadBroadcasted = true;
            }
        }
        
        virtual void OnAssetLoadFinished() = 0;

        void OnSave() override;

    protected:

        CObject*                    Asset;
        uint8                       bAssetLoadBroadcasted:1;

    };
}
