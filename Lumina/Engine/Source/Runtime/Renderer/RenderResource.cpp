#include "RenderResource.h"

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

    FRHIImage::FRHIImage(const FRHIImageDesc& InDesc)
        :Description(InDesc)
    {
    }
}
