#include "Asset.h"


namespace Lumina
{
    LAsset::LAsset(EAssetType InType, std::filesystem::path InPath)
    {
        AssetType = InType;
        FilePath = InPath;
    }

    LAsset::~LAsset()
    {
        
    }
}
