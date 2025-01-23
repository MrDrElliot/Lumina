#include "Texture.h"

namespace Lumina
{
    LTexture::LTexture()
    {
    }

    LTexture::~LTexture()
    {
        Image = nullptr;
    }

    void LTexture::Serialize(FArchive& Ar)
    {
        LAsset::Serialize(Ar);
        Ar << ImageSpec;
    }

    void LTexture::CreateImage()
    {
        Image = FImage::Create(ImageSpec);
        Image->SetFriendlyName("Image: " + GetAssetMetadata().Name);
    }
}
