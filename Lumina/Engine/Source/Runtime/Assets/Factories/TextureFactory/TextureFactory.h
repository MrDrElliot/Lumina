#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"
#include "Renderer/RHIFwd.h"
#include "TextureFactory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CTextureFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        FString GetAssetName() const override { return "Texture"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewTexture"; }

        void TryImport(const FString& RawPath, const FString& DestinationPath) override;
        
    };
}
