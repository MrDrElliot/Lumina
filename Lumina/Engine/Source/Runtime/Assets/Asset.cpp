#include "Asset.h"

namespace Lumina
{
    FAsset::FAsset()
    {
        AssetType = EAssetType::StaticMesh;
      //  Guid = FGuid::Generate();
    }

    FAsset::~FAsset()
    {
        
    }
}
