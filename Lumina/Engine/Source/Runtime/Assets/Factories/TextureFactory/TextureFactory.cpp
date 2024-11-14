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

    TFastVector<uint8> FTextureFactory::ImportFromSource(std::filesystem::path Path)
    {
        stbi_set_flip_vertically_on_load(true);

        int x, y, c;
        stbi_uc* data = stbi_load(Path.string().c_str(), &x, &y, &c, STBI_rgb_alpha);
        AssertMsg(data, "Failed to import texture!");

        // Move the raw data directly to TFastVector to avoid copying
        TFastVector<uint8> out;
        out.assign(data, data + (x * y * STBI_rgb_alpha));

        stbi_image_free(data); // Free the image data

        return out;
    }
}
