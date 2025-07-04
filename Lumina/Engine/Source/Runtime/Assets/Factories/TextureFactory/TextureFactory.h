#pragma once
#include "Assets/Factories/Factory.h"
#include "Memory/RefCounted.h"
#include "Renderer/RHIFwd.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "TextureFactory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CTextureFactory : public CFactory
    {
        GENERATED_BODY()
    public:
        
        CClass* GetSupportedType() const override { return CTexture::StaticClass(); }
        FString GetAssetName() const override { return "Texture"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewTexture"; }

        void TryImport(const FString& RawPath, const FString& DestinationPath) override;
        
    };
}
