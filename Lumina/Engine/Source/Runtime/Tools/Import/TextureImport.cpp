
#include "ImportHelpers.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderResource.h"
#include "stb_image/stb_image.h"

namespace Lumina::Import::Textures
{
    FIntVector2D ImportTexture(TVector<uint8>& OutPixels, const FString& RawFilePath, bool bFlipVertical)
    {
        OutPixels.clear();
        
        stbi_set_flip_vertically_on_load(bFlipVertical);

        int x, y, c;
        stbi_uc* data = stbi_load(RawFilePath.c_str(), &x, &y, &c, STBI_rgb_alpha);
        if (data == nullptr)
        {
            LOG_WARN("Failed to get image pixel data from: {0}", RawFilePath);
            return {};
        }
        
        OutPixels.assign(data, data + static_cast<size_t>(x) * static_cast<size_t>(y) * STBI_rgb_alpha);
        stbi_image_free(data);
        
        return {x, y};
    }

    FRHIImageRef CreateTextureFromImport(IRenderContext* RenderContext, const FString& RawFilePath, bool bFlipVerticalOnLoad)
    {
        TVector<uint8> Pixels;
        FRHIImageDesc ImageDescription;
        ImageDescription.Format = EFormat::RGBA8_UNORM;
        ImageDescription.Extent = ImportTexture(Pixels, RawFilePath, bFlipVerticalOnLoad);
        ImageDescription.Flags.SetFlag(EImageCreateFlags::ShaderResource);
        ImageDescription.NumMips = 1;
        ImageDescription.DebugName = Paths::FileName(RawFilePath, true);
        ImageDescription.InitialState = EResourceStates::ShaderResource;
        ImageDescription.bKeepInitialState = true;
        
        FRHIImageRef ReturnImage = RenderContext->CreateImage(ImageDescription);

        const uint32 Width = ImageDescription.Extent.X;
        const uint32 Height = ImageDescription.Extent.Y;
        const SIZE_T RowPitch = Width * 4;
        const SIZE_T DepthPitch = RowPitch * Height;

        FRHICommandListRef TransferCommandList = RenderContext->CreateCommandList(FCommandListInfo::Graphics());
        TransferCommandList->Open();
        TransferCommandList->WriteImage(ReturnImage, 0, 0, Pixels.data(), RowPitch, DepthPitch);
        TransferCommandList->Close();
        RenderContext->ExecuteCommandList(TransferCommandList, ECommandQueue::Graphics);

        return ReturnImage;
    }
    
}