#include "Asset.h"

#include "Core/Serialization/Archiver.h"


namespace Lumina
{
    LAsset::LAsset(const FAssetMetadata& InMetadata)
        :Metadata(InMetadata)
    {
    }

    LAsset::LAsset(const std::string& AssetName)
    {
        Metadata.Name = AssetName;
    }
    void LAsset::Serialize(FArchive& Ar)
    {
        Ar << Metadata;
    }
}
