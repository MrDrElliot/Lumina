#include "TextureFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image/stb_image.h>

#include "Assets/AssetRecord.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Performance/PerformanceTracker.h"

namespace Lumina
{
    
    ELoadResult FTextureFactory::CreateNew(const FAssetHandle& InHandle, const FAssetPath& InPath, FAssetRecord* InRecord, FArchive& Archive)
    {
        auto Texture = MakeSharedPtr<ATexture>();
        Texture->Serialize(Archive);
        Texture->CreateImage();
        
        return ELoadResult::Succeeded;
    }

    TRefPtr<FImage> FTextureFactory::ImportFromSource(std::filesystem::path Path)
    {
        PROFILE_SCOPE_LOG(FTextureFactory::ImportFromSource)

        FImageSpecification ImageSpec = FImageSpecification::Default();
        ImageSpec.Usage = EImageUsage::TEXTURE;
        ImageSpec.Format = EImageFormat::RGBA32_UNORM;
        ImageSpec.MipLevels = 1;

        stbi_set_flip_vertically_on_load(true);

        int x, y, c;
        stbi_uc* data = stbi_load(Path.string().c_str(), &x, &y, &c, STBI_rgb_alpha);
        if (data == nullptr)
        {
            LOG_WARN("Failed to import texture from source path: {0}", Path.string());
            return nullptr;
        }

        ImageSpec.Extent.x = x;
        ImageSpec.Extent.y = y;
        ImageSpec.SourceChannels = c;
        ImageSpec.Pixels.assign(data, data + (x * y * STBI_rgb_alpha));

        stbi_image_free(data);
        
        TRefPtr<FImage> Image = FImage::Create(ImageSpec);

        return Image;
    }
}
