#include "Texture.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"

namespace Lumina
{
    void CTexture::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
        Ar << ImageDescription;
        Ar << Pixels;
    }

    void CTexture::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CTexture::PostLoad()
    {
        FRenderManager* RenderManager = GEngine->GetEngineSubsystem<FRenderManager>();
        IRenderContext* RenderContext = RenderManager->GetRenderContext();

        RHIImage = RenderContext->CreateImage(ImageDescription);

        const uint32 Width = ImageDescription.Extent.X;
        const uint32 Height = ImageDescription.Extent.Y;
        const SIZE_T RowPitch = Width * 4; // 4 bytes per pixel (RGBA8)
        const SIZE_T DepthPitch = RowPitch * Height; // 2D texture → no slices

        FRHICommandListRef TransferCommandList = RenderContext->CreateCommandList({ECommandQueue::Transfer});
        TransferCommandList->Open();
        TransferCommandList->WriteToImage(RHIImage, 0, 0, Pixels.data(), RowPitch, DepthPitch);
        TransferCommandList->Close();
        RenderContext->ExecuteCommandList(TransferCommandList, 1, Q_Transfer);
    }
}
