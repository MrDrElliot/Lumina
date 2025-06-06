#include "TextureFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image/stb_image.h>

#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    /*ATexture FTextureFactory::ImportFromSource(std::filesystem::path Path)
    {
        PROFILE_SCOPE_LOG(FTextureFactory::ImportFromSource)

        FImageSpecification ImageSpec;
        ImageSpec.Usage = EImageUsage::TEXTURE;
        ImageSpec.Format = EImageFormat::RGBA32_UNORM;
        ImageSpec.MipLevels = 1;

        stbi_set_flip_vertically_on_load(true);

        int x, y, c;
        stbi_uc* data = stbi_load(Path.string().c_str(), &x, &y, &c, STBI_rgb_alpha);
        if (data == nullptr)
        {
            LOG_WARN("Failed to import texture from source path: {0}", Path.string());
            return FRHIImageHandle();
        }

        FIntVector2D Extent;
        Extent.X = x;
        Extent.Y = y;
        
        FRHIImageHandle ImageHandle = Manager->GetRenderContext()->CreateTexture(Extent);

        ImageSpec.Extent = Extent;
        ImageSpec.SourceChannels = c;
        ImageSpec..assign(data, data + (x * y * STBI_rgb_alpha));

        stbi_image_free(data);

        return ImageHandle;
    }*/
    
    CObject* CTextureFactory::CreateNew(const FString& Path)
    {
        return {};
    }

    void CTextureFactory::CreateAssetFile(const FString& Path)
    {
        CFactory::CreateAssetFile(Path);
    }
}
