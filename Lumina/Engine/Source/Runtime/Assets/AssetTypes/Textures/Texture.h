#pragma once

#include "Assets/Asset.h"
#include "Memory/RefCounted.h"
#include "Renderer/Image.h"


namespace Lumina
{

    class LTexture : public LAsset
    {
    public:
        
        LTexture();
        ~LTexture();

        void Serialize(FArchive& Ar) override;
        void CreateImage();
        TRefPtr<FImage> GetImage() { return Image; }

        void SetImage(const TRefPtr<FImage>& InImage, const FImageSpecification& Spec);
    
    private:
        
        FImageSpecification ImageSpec;
        TRefPtr<FImage> Image;
    };
}
