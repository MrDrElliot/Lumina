#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"
#include "Renderer/RHIFwd.h"


namespace Lumina
{
    class FImage;

    class FTextureFactory : public FFactory
    {
    public:

        ELoadResult CreateNew(FAssetRecord* InRecord) override;
        //static FRHIImageHandle ImportFromSource(std::filesystem::path Path);

    };
}
