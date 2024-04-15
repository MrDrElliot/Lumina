#pragma once
#include "AssetTypes.h"
#include "Source/Runtime/CoreObject/Object.h"
#include "Source/Runtime/GUID/GUID.h"

namespace Lumina
{
    class LAsset : public LObject
    {
    public:

        LAsset(EAssetType InType);
        virtual ~LAsset();


        EAssetType GetAssetType() const { return AssetType; }
        const std::string& GetFilePath() const { return FilePath; }
        
    private:

        EAssetType AssetType;
        std::string FilePath;
    };
}
