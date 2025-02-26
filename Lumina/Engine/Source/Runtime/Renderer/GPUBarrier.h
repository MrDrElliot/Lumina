#pragma once
#include "RenderTypes.h"
#include "RHIFwd.h"

namespace Lumina
{
    enum class EImageLayout;
}

namespace Lumina
{

    struct FGPUBarrier
    {
        enum class EType : uint8
        {
            Memory,
            Image,
        };

        FGPUBarrier& Memory()
        {
            return *this;
        }

        static FGPUBarrier Image(FRHIImageHandle InImage, EImageLayout From, EImageLayout To = EImageLayout::Default)
        {
            FGPUBarrier Barrier;
            Barrier.Type = EType::Image;
            Barrier.RHIImage = InImage;
            Barrier.FromLayout = From;
            Barrier.ToLayout = To;

            return Barrier;
        }
        
        EType Type;
        FRHIImageHandle RHIImage;
        EImageLayout FromLayout;
        EImageLayout ToLayout;
    };


}
