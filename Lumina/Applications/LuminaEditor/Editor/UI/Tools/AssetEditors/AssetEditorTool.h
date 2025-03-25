#pragma once

#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "UI/Tools/EditorTool.h"
#include "UI/Tools/EditorToolContext.h"

namespace Lumina
{
    class FAssetEditorTool : public FEditorTool
    {
    public:
        
        FAssetEditorTool(IEditorToolContext* Context, const FString& AssetName, const FAssetPath& InAsset)
            : FEditorTool(Context, AssetName, nullptr)
        {
            bAssetLoadBroadcasted = false;
            FAssetManager* AssetManager = Context->GetSubsystemManager()->GetSubsystem<FAssetManager>();
            FAssetHeader Header = Context->AssetRegistry->FindAssetHeader(InAsset);
            FAssetHandle Handle(InAsset, Header.Type);
            Asset = AssetManager->LoadAsset(Handle);
        }

        void Update(const FUpdateContext& UpdateContext) override
        {
            if ((!bAssetLoadBroadcasted) && Asset->IsLoaded() || Asset->HasLoadingFailed())
            {
                OnAssetLoadFinished();
                bAssetLoadBroadcasted = true;
            }
        }

        virtual void OnAssetLoadFinished() = 0;

    protected:

        TRefCountPtr<FAssetRecord>      Asset;
        uint8 bAssetLoadBroadcasted:1;

    };
}
