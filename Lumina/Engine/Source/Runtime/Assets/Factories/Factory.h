#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"



namespace Lumina
{
    class FAssetRecord;
    
    class FFactory
    {
    public:

        virtual ~FFactory() = default;

       virtual ELoadResult CreateNew(const FAssetHandle& InHandle, const FAssetPath& InPath, FAssetRecord* InRecord, FArchive& Archive) = 0;
    };
}
