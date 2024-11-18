#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
    class FImage;

    class FTextureFactory : public FFactory
    {
    public:

        std::shared_ptr<LAsset> CreateNew(const FAssetMetadata& Metadata, FArchive& Archive) override;
        static TRefPtr<FImage> ImportFromSource(std::filesystem::path Path);

    };
}
