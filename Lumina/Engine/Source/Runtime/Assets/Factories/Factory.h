#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"


namespace Lumina
{
    class FFactory
    {
    public:

       virtual TSharedPtr<LAsset> CreateNew(const FAssetMetadata& Metadata, FArchive& Archive) = 0;
    };
}
