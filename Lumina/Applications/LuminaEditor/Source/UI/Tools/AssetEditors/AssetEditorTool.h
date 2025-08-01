﻿#pragma once

#include "Core/Object/Class.h"
#include "Core/Object/Object.h"
#include "UI/Properties/PropertyTable.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "UI/Tools/EditorTool.h"

namespace Lumina
{
    class FAssetEditorTool : public FEditorTool
    {
    public:
        
        FAssetEditorTool(IEditorToolContext* Context, const FString& AssetName, CObject* InAsset)
            : FEditorTool(Context, AssetName, nullptr)
            , bAssetLoadBroadcasted(false)
            , PropertyTable(FPropertyTable(InAsset, InAsset->GetClass()))
        {
            Asset = InAsset;
            PropertyTable.RebuildTree();
        }

        void Deinitialize(const FUpdateContext& UpdateContext) override;
        void Update(const FUpdateContext& UpdateContext) override;
        FPropertyTable* GetPropertyTable() { return &PropertyTable; }
        
        FString GetToolName() const override;
        virtual void OnAssetLoadFinished() { }
        void OnSave() override;
        

    protected:

        TObjectHandle<CObject>      Asset;
        uint8                       bAssetLoadBroadcasted:1;
        FPropertyTable              PropertyTable;
    };
}
