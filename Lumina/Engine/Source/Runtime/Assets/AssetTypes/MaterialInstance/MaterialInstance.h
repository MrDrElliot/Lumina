#pragma once
#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Renderer/RenderTypes.h"




namespace Lumina
{
    class ATexture;

    class AMaterialInstance : public IAsset
    {
    public:

        DECLARE_ASSET("Material Instance", MaterialInstance, 1)
        
        void Serialize(FArchive& Ar) override;

        TAssetHandle<ATexture> Albedo;
        TAssetHandle<ATexture> Normal;
        TAssetHandle<ATexture> Roughness;
        TAssetHandle<ATexture> Emissive;
        TAssetHandle<ATexture> AmbientOcclusion;

        FMaterialTexturesData MaterialTextureIDs;

    
    private:
        
    };
}
