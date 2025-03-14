#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"
#include "Renderer/RHIFwd.h"


namespace Lumina
{
    class FTextureFactory : public FFactory
    {
    public:

        ELoadResult LoadFromDisk(FAssetRecord* InRecord) override;
        FAssetPath CreateNew(const FString& Path) override;
        //static FRHIImageHandle ImportFromSource(std::filesystem::path Path);

        const FString& GetAssetName() const override { return "Texture"; }
    };
}
