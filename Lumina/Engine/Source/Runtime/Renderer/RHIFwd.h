#pragma once

#include "RHIFwd.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
    class IRHIInputLayout;
    class FRHIBindingSet;
    class FRHIBindingLayout;
    class FRHIComputePipeline;
}

namespace Lumina
{
    class FRHIGraphicsPipeline;
}

namespace Lumina
{
    class FRHIViewport;
    class ICommandList;
    class FRHIComputeShader;
    class FRHIPixelShader;
    class FRHIVertexShader;
    class FRHIBuffer;
    class FRHIImage;
    
    //----------------------------------------------------------------------------

    using FRHIBufferRef             = TRefCountPtr<FRHIBuffer>;
    using FRHIImageRef              = TRefCountPtr<FRHIImage>;
    using FRHIVertexShaderRef       = TRefCountPtr<FRHIVertexShader>;
    using FRHIPixelShaderRef        = TRefCountPtr<FRHIPixelShader>;
    using FRHIComputeShaderRef      = TRefCountPtr<FRHIComputeShader>;
    using FRHICommandListRef        = TRefCountPtr<ICommandList>;
    using FRHIViewportRef           = TRefCountPtr<FRHIViewport>;
    using FRHIGraphicsPipelineRef   = TRefCountPtr<FRHIGraphicsPipeline>;
    using FRHIComputePipelineRef    = TRefCountPtr<FRHIComputePipeline>;
    using FRHIBindingLayoutRef      = TRefCountPtr<FRHIBindingLayout>;
    using FRHIBindingSetRef         = TRefCountPtr<FRHIBindingSet>;
    using FRHIInputLayoutRef        = TRefCountPtr<IRHIInputLayout>;

}
