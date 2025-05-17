#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"
#include "Renderer/RHIFwd.h"


namespace Lumina
{
    LUM_CLASS()
    class CTextureFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CObject* CreateNew(const FString& Path) override;
        //static FRHIImageHandle ImportFromSource(std::filesystem::path Path);

        FString GetAssetName() const override { return "Texture"; }

        void CreateAssetFile(const FString& Path) override;
    };
}
