#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"



namespace Lumina
{
    class FFactory
    {
    public:
        
        virtual ~FFactory() = default;
        virtual ELoadResult CreateNew(FAssetHandle& InHandle) = 0;
        virtual ELoadResult UpdateInProcessRequest(FAssetHandle& InHandle) = 0;
    };
}
