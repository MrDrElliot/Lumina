#pragma once
#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Core/Math/Color.h"
#include "Core/Math/Math.h"

namespace Lumina
{

    struct FRenderPassBeginInfo
    {
        
        TVector<FRHIImage> Attachments;
        
        bool bClearValue;
        FColor ClearColor;
        
        FIntVector2D RenderAreaOffset;
        FIntVector2D RenderAreaExtent;
    };
    
    class IRenderPass
    {
    public:

    private:
    };
}
