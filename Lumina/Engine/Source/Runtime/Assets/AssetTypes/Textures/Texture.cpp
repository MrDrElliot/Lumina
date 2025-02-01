#include "Texture.h"

namespace Lumina
{
    void ATexture::Serialize(FArchive& Ar)
    {
        Ar << ImageSpec;
    }

    void ATexture::CreateImage()
    {
        Image = FImage::Create(ImageSpec);
        Image->SetFriendlyName("Image: ");
    }

    void ATexture::SetImage(const TRefPtr<FImage>& InImage, const FImageSpecification& Spec)
    {
        ImageSpec = Spec;
        Image = InImage;
    }
}
