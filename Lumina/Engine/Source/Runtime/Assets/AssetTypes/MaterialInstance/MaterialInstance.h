#pragma once
#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Renderer/RenderTypes.h"


namespace Lumina
{
    class LTexture;
}

namespace Lumina
{
    class LMaterialInstance : public LAsset
    {
    public:

        LMaterialInstance()
        {
            Attributes = {};
        }

        FMaterialAttributes& GetMaterialAttributes() { return Attributes; }
        void Serialize(FArchive& Ar) override;

        TAssetHandle<LTexture> Albedo;
        TAssetHandle<LTexture> Normal;
        TAssetHandle<LTexture> Roughness;
        TAssetHandle<LTexture> Emissive;
        TAssetHandle<LTexture> AmbientOcclusion;

        FMaterialTexturesData MaterialTextureIDs;

    
    private:

        
        FMaterialAttributes Attributes;
    };
}
