#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
    class FImage;

    class FTextureFactory : public FFactory
    {
    public:

        ELoadResult CreateNew(FAssetRecord* InRecord) override;
        static TRefPtr<FImage> ImportFromSource(std::filesystem::path Path);

    };
}
