#pragma once
#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Core/Math/Color.h"
#include "Core/Math/Math.h"

namespace Lumina
{

    struct FRenderPassBeginInfo
    {
        
        TInlineVector<FRHIImage, 4> Attachments;
        FIntVector2D RenderAreaOffset;
        FIntVector2D RenderAreaExtent;
        FColor ClearColor;
        bool bClearValue;

        FRenderPassBeginInfo& AddAttachment(FRHIImage Image)
        {
            Attachments.push_back(Image);

            return *this;
        }

        FRenderPassBeginInfo& SetRenderAreaOffset(FIntVector2D Offset)
        {
            RenderAreaOffset = Offset;

            return *this;
        }

        FRenderPassBeginInfo& SetRenderAreaExtent(FIntVector2D Extent)
        {
            RenderAreaExtent = Extent;

            return *this;
        }

        FRenderPassBeginInfo& SetClearColor(const FColor& Color)
        {
            ClearColor = Color;

            return *this;
        }

        FRenderPassBeginInfo& SetClearValue(bool bClear)
        {
            bClearValue = bClear;

            return *this;
        }
        
    };
    
    class IRenderPass
    {
    public:

    private:
    };
}
