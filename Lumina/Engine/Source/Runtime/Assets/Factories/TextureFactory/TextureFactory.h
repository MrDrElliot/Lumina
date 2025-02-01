#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
    class FImage;

    class FTextureFactory : public FFactory
    {
    public:

        ELoadResult CreateNew(const FAssetHandle& InHandle, const FAssetPath& InPath, FAssetRecord* InRecord, FArchive& Archive) override;
        static TRefPtr<FImage> ImportFromSource(std::filesystem::path Path);

    };
}
