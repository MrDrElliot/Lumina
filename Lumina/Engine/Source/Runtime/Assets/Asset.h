#pragma once

#include <filesystem>
#include "AssetTypes.h"



namespace Lumina
{
    class LAsset : public std::enable_shared_from_this<LAsset>
    {
    public:

        LAsset() = default;
        LAsset(const FAssetMetadata& InMetadata);
        LAsset(const std::string& AssetName);
        virtual ~LAsset() = default;
        
        static EAssetType GetStaticType() { return EAssetType::None; }
        virtual void Serialize(FArchive& Ar);

        
    public:

        FAssetMetadata& GetAssetMetadata() { return Metadata; }
        EAssetType GetAssetType() const { return Metadata.AssetType; }
        const std::filesystem::path& GetFilePath() const { return Metadata.Path; }
        
    private:

        FAssetMetadata Metadata;
    };
}
