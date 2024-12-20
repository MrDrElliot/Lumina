#include "TextureFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image/stb_image.h>

#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/LuminaMacros.h"

namespace Lumina
{
    std::shared_ptr<LAsset> FTextureFactory::CreateNew(const FAssetMetadata& Metadata, FArchive& Archive)
    {
        // Deserialize the texture from Archive
        auto Texture = std::make_shared<LTexture>();
        Texture->Serialize(Archive);
        Texture->CreateImage();

        return Texture;
        
    }

    TRefPtr<FImage> FTextureFactory::ImportFromSource(std::filesystem::path Path)
    {
        FImageSpecification ImageSpec = FImageSpecification::Default();
        ImageSpec.Usage = EImageUsage::TEXTURE;
        ImageSpec.Format = EImageFormat::RGBA32_UNORM;
        ImageSpec.MipLevels = 1;

        stbi_set_flip_vertically_on_load(true);

        int x, y, c;
        stbi_uc* data = stbi_load(Path.string().c_str(), &x, &y, &c, STBI_rgb_alpha);
        AssertMsg(data, "Failed to import texture!");

        ImageSpec.Extent.x = x;
        ImageSpec.Extent.y = y;
        ImageSpec.SourceChannels = c;
        ImageSpec.Pixels.assign(data, data + (x * y * STBI_rgb_alpha));

        stbi_image_free(data); // Free the image data
        
        return FImage::Create(ImageSpec);
    }
}
