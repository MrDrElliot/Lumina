#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"



namespace Lumina
{
    class FFactory
    {
    public:
        
        virtual ~FFactory() = default;
        virtual ELoadResult CreateNew(FAssetRecord* InRecord) = 0;
        virtual ELoadResult UpdateInProcessRequest(FAssetRecord* InRecord) { return ELoadResult::Succeeded; };
    };
}
