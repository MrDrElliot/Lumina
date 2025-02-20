#pragma once

#include "Memory/RefCounted.h"


namespace Lumina
{
    class FDescriptorSet;
    class IRenderResource;
    class FShader;
    class FBuffer;
    class FPipeline;
    class FSemaphore;
    class FSemaphore;
    class FCommandBuffer;
    class FSwapchain;
    class FImageSampler;
    class FImage;

    //---------------------------------------------------------------------------
    
    using FRHIResource          = TRefCountPtr<IRenderResource>;
    using FRHIShader            = TRefCountPtr<FShader>;
    using FRHIBuffer            = TRefCountPtr<FBuffer>;
    using FRHIPipeline          = TRefCountPtr<FPipeline>;
    using FRHIImage             = TRefCountPtr<FImage>;
    using FRHIImageSampler      = TRefCountPtr<FImageSampler>;
    using FRHISwapchain         = TRefCountPtr<FSwapchain>;
    using FRHIDescriptorSet     = TRefCountPtr<FDescriptorSet>;
    using FRHICommandBuffer     = TRefCountPtr<FCommandBuffer>;
    using FRHISemaphore         = TRefCountPtr<FSemaphore>;
}
