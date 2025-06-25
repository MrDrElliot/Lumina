#pragma once
#include "Assets/Definition/AssetDefinition.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "AssetDefinition_Texture.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CAssetDefinition_Texture : public CAssetDefinition
    {
        GENERATED_BODY()
        
    public:

        FString GetImportFileExtension() override { return ".png"; }
        CFactory* GetFactory() const override { return GetMutableDefault<CTextureFactory>(); }
        CClass* GetAssetClass() const override { return CTexture::StaticClass(); }
        bool CanImport() override { return true; }
        
        
    };
}
