#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"



namespace Lumina
{
    class FFactory
    {
    public:
        
        virtual ~FFactory() = default;
        virtual ELoadResult LoadFromDisk(FAssetRecord* InRecord) = 0;
        virtual IAsset* CreateNew(const FString& Path) = 0;
        virtual ELoadResult UpdateInProcessRequest(FAssetRecord* InRecord) { return ELoadResult::Succeeded; }
        
        virtual const FString& GetAssetName() const = 0;
        
    };
}
