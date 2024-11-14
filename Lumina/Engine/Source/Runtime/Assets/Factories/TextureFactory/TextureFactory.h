#pragma once
#include "Assets/Factories/Factory.h"


namespace Lumina
{
    
    class FTextureFactory : public FFactory
    {
    public:

        std::shared_ptr<LAsset> CreateNew(const FAssetMetadata& Metadata, FArchive& Archive) override;
        static TFastVector<uint8> ImportFromSource(std::filesystem::path Path);

    };
}
