#include "Texture.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"

namespace Lumina
{
    void CTexture::Serialize(FArchive& Ar)
    {
        Ar << ImageDescription;
        Ar << Pixels;
    }

    void CTexture::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CTexture::UpdateStreamableResource()
    {
        FRenderManager* RenderManager = GEngine->GetEngineSubsystem<FRenderManager>();
        IRenderContext* RenderContext = RenderManager->GetRenderContext();
        RHIImage = RenderContext->CreateImage(ImageDescription);
        RenderContext->GetCommandList()->SetRequiredImageAccess(RHIImage, ERHIAccess::HostRead);
    }
}
