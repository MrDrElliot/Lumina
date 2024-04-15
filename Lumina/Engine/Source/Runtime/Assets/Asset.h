#pragma once
#include "AssetTypes.h"
#include "Source/Runtime/GUID/GUID.h"

namespace Lumina
{
    class FAsset
    {
    public:

        FAsset();
        virtual ~FAsset();


        EAssetType GetAssetType() const { return AssetType; }
       // FGuid GetGuid() const { return Guid; }
        const std::string& GetFilePath() const { return FilePath; }
        
    private:

       // FGuid Guid;
        EAssetType AssetType;
        std::string FilePath;
    };
}
