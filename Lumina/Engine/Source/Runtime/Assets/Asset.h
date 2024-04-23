#pragma once
#include <filesystem>

#include "AssetTypes.h"
#include "Source/Runtime/CoreObject/Object.h"
#include "Source/Runtime/GUID/GUID.h"

namespace Lumina
{
    class LAsset : public LObject
    {
    public:

        LAsset(EAssetType InType, std::filesystem::path InPath);
        virtual ~LAsset();


        EAssetType GetAssetType() const { return AssetType; }
        const std::filesystem::path& GetFilePath() const { return FilePath; }
        
    private:
        
        EAssetType AssetType;
        std::filesystem::path FilePath;
    };
}
