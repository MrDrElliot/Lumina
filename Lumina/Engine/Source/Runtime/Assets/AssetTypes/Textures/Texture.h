#pragma once

#include "Assets/Asset.h"
#include "Memory/RefCounted.h"


namespace Lumina
{

    class ATexture : public IAsset
    {
    public:
        
        ATexture(const FAssetPath& InPath)
            : IAsset(InPath)
        {}

        DECLARE_ASSET("Texture", Texture, 1)
        
        void Serialize(FArchive& Ar) override;
        void PostLoad() override;
        
    
    private:
        
 
    };
}
