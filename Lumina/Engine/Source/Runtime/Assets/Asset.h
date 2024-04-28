#pragma once
#include <filesystem>

#include "AssetTypes.h"
#include "Source/Runtime/CoreObject/Object.h"
#include "Source/Runtime/GUID/GUID.h"

namespace Lumina
{

    struct FAssetMetadata
    {
        std::string Name;
        FGuid Guid;
        std::filesystem::path Path;
        std::filesystem::path OriginPath;
        bool bLoaded = false;
        EAssetType AssetType;
    };
    
    class LAsset : public LObject
    {
    public:

        LAsset(const FAssetMetadata& InMetadata);
        virtual ~LAsset();


    public:

        FAssetMetadata& GetAssetMetadata() { return Metadata; }
        EAssetType GetAssetType() const { return Metadata.AssetType; }
        const std::filesystem::path& GetFilePath() const { return Metadata.Path; }
        const FGuid GetGUID() const { return Metadata.Guid; }
        
    private:

        FAssetMetadata Metadata;
    };
}
