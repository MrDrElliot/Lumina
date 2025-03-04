#include "RenderResource.h"

#include "RenderContext.h"
#include "Containers/Array.h"


namespace Lumina
{

    TStack<IRHIResource*> PendingDeletes;
    
    void IRHIResource::Destroy() const
    {
        if (!AtomicFlags.MarkForDelete(std::memory_order_relaxed))
        {
            PendingDeletes.push(const_cast<IRHIResource*>(this));
        }
    }
    
    void FRHIViewport::SetSize(const FIntVector2D& InSize)
    {
        if (Size == InSize)
        {
            return;
        }
        
        CreateRenderTarget(InSize);
    }

    void FRHIViewport::CreateRenderTarget(const FIntVector2D& InSize)
    {
        FRHIImageDesc Desc;
        Desc.Format = EImageFormat::BGRA32_UNORM;
        Desc.Flags.SetFlag(EImageCreateFlags::RenderTarget);
        Desc.Extent = InSize;

        RenderTarget = RenderContext->CreateImage(Desc);
    }
}
