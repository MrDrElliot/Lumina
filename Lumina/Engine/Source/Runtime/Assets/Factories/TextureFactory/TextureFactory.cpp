#include "TextureFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image/stb_image.h>

#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    ELoadResult FTextureFactory::CreateNew(FAssetRecord* InRecord)
    {
        ATexture* Texture = new ATexture(InRecord->GetAssetPath());

        TVector<uint8> Buffer;
        if (!FFileHelper::LoadFileToArray(Buffer, InRecord->GetAssetPath().GetPathAsString()))
        {
            return ELoadResult::Failed;
        }

        FMemoryReader Reader(Buffer);

        FAssetHeader Header;
        Reader << Header;

        if (Header.Type != InRecord->GetAssetType())
        {
            return ELoadResult::Failed;
        }

        InRecord->SetDependencies(eastl::move(Header.Dependencies));
        
        Texture->Serialize(Reader);
        InRecord->SetAssetPtr(Texture);
        
        return ELoadResult::Succeeded;
    }

    FRHIImage FTextureFactory::ImportFromSource(std::filesystem::path Path)
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

        ImageSpec.Extent.X = x;
        ImageSpec.Extent.Y = y;
        ImageSpec.SourceChannels = c;
        ImageSpec.Pixels.assign(data, data + (x * y * STBI_rgb_alpha));

        stbi_image_free(data);
        
        FRHIImage Image = FImage::Create(ImageSpec);

        return Image;
    }
}
