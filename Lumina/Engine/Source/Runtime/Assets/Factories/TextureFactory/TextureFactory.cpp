#include "TextureFactory.h"

#include "Assets/AssetHeader.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Core/Object/Package/Package.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RenderTypes.h"
#include "Tools/Import/ImportHelpers.h"

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

    CObject* CTextureFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        return NewObject<CTexture>(Package, Name);
    }

    void CTextureFactory::TryImport(const FString& RawPath, const FString& DestinationPath)
    {
        FString FullPath = DestinationPath;
        Paths::AddPackageExtension(FullPath);
        FString VirtualPath = Paths::ConvertToVirtualPath(DestinationPath);
        FString FileName = Paths::FileName(DestinationPath, true);
        
        std::filesystem::path FilePath = RawPath.c_str();


        CTexture* NewTexture = Cast<CTexture>(TryCreateNew(DestinationPath));
        NewTexture->SetFlag(OF_NeedsPostLoad);
        
        FRHIImageDesc ImageDescription;
        ImageDescription.Format = EFormat::RGBA8_UNORM;
        ImageDescription.Extent = Import::Textures::ImportTexture(NewTexture->Pixels, RawPath);
        ImageDescription.Flags.SetFlag(EImageCreateFlags::ShaderResource);
        ImageDescription.NumMips = CalculateMipCount(ImageDescription.Extent.X, ImageDescription.Extent.Y);
        ImageDescription.InitialState = EResourceStates::ShaderResource;
        ImageDescription.bKeepInitialState = true;
        NewTexture->ImageDescription = ImageDescription;

        if (ImageDescription.Extent.X == 0 || ImageDescription.Extent.Y == 0)
        {
            LOG_ERROR("Attempted to import an image with an invalid size: X: {} Y: {}", ImageDescription.Extent.X, ImageDescription.Extent.Y);
        }
        
    }
}
