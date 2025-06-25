#include "TextureFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image/stb_image.h>

#include "Assets/AssetHeader.h"
#include "Assets/AssetPath.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Object/Cast.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Core/Serialization/Package/PackageSaver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    static uint8 CalculateMipCount(uint32 width, uint32 height)
    {
        uint32 levels = 1;
        while (width > 1 || height > 1)
        {
            width = std::max(width >> 1, 1u);
            height = std::max(height >> 1, 1u);
            ++levels;
        }
        return static_cast<uint8>(levels);
    }
    
    void CTextureFactory::CreateAssetFile(const FString& Path)
    {
        FString FullPath = Path + ".lasset";
        if (!FFileHelper::CreateNewFile(FullPath, true))
        {
            LOG_INFO("Failed to created New Texture: {}", FullPath);
            return;
        }

        FString VirtualPath = Paths::ConvertToVirtualPath(Path);

        FAssetHeader Header;
        Header.Path = VirtualPath;
        Header.ClassName = "CTexture";
        Header.Type = EAssetType::Texture;
        Header.Version = 1;

        TVector<uint8> Buffer;
        FPackageSaver Saver(Buffer);
        Saver << Header;

        CObject* Temp = NewObject<CTexture>(UTF8_TO_WIDE(VirtualPath).c_str());

        FBinaryStructuredArchive BinaryAr(Saver);
        Temp->Serialize(BinaryAr.Open());
        
        FFileHelper::SaveArrayToFile(Buffer, FullPath);

        Temp->DestroyNow();
        
    }

    void CTextureFactory::TryImport(const FString& RawPath, const FString& DestinationPath)
    {
        FString FullPath = DestinationPath + ".lasset";
        FString VirtualPath = Paths::ConvertToVirtualPath(DestinationPath);

        std::filesystem::path FilePath = RawPath.c_str();
        Assert(FilePath.extension() == ".png")

        CTexture* Temp = NewObject<CTexture>(UTF8_TO_WIDE(DestinationPath).c_str());
        
        CreateAssetFile(DestinationPath);


        stbi_set_flip_vertically_on_load(true);

        int x, y, c;
        stbi_uc* data = stbi_load(RawPath.c_str(), &x, &y, &c, STBI_rgb_alpha);
        if (data == nullptr)
        {
            LOG_WARN("Failed to import texture from source path: {0}", RawPath);
            return;
        }

        FIntVector2D Extent;
        Extent.X = x;
        Extent.Y = y;
        
        FRHIImageDesc ImageDescription;
        ImageDescription.Extent = Extent;
        ImageDescription.Format = EImageFormat::RGBA32_UNORM;
        ImageDescription.Flags.SetFlag(EImageCreateFlags::ShaderResource);
        ImageDescription.NumMips = CalculateMipCount(Extent.X, Extent.Y);

        Temp->ImageDescription = ImageDescription;
        Temp->Pixels.assign(data, data + static_cast<size_t>(x) * static_cast<size_t>(y) * STBI_rgb_alpha);
        
        stbi_image_free(data);
        
        FAssetHeader Header;
        Header.Path = VirtualPath;
        Header.ClassName = "CTexture";
        Header.Type = EAssetType::Texture;
        Header.Version = 1;
        
        TVector<uint8> Buffer;
        FPackageSaver Saver(Buffer);
        Saver << Header;
        
        FBinaryStructuredArchive BinaryAr(Saver);
        Temp->Serialize(BinaryAr.Open());
        
        FFileHelper::SaveArrayToFile(Buffer, FullPath);
        
        Temp->DestroyNow();
    }
}
