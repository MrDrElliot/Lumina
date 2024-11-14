#pragma once
#include <any>

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"


namespace Lumina
{
    class FFactory
    {
    public:

       virtual std::shared_ptr<LAsset> CreateNew(const FAssetMetadata& Metadata, FArchive& Archive) = 0;
    };
}
