#include "Texture.h"

namespace Lumina
{
    void ATexture::Serialize(FArchive& Ar)
    {
        Ar << ImageSpec;
    }

    void ATexture::PostLoad()
    {
        Image = FImage::Create(ImageSpec);
        Image->SetFriendlyName("Image: ");
    }
}
