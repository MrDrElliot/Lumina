#pragma once

#include "Memory/RefCounted.h"


namespace Lumina
{
    class FRHIDescriptorTable;
    class IEventQuery;
    class FRHISampler;
    class IRHIResource;
    class FShaderLibrary;
    class FRHIShader;
    class IRHIInputLayout;
    class FRHIBindingSet;
    class FRHIBindingLayout;
    class FRHIComputePipeline;
    class FRHIGraphicsPipeline;
    class FRHIViewport;
    class ICommandList;
    class FRHIComputeShader;
    class FRHIPixelShader;
    class FRHIVertexShader;
    class FRHIBuffer;
    class FRHIImage;
    
    //----------------------------------------------------------------------------

    using FRHIEventQueryRef         = TRefCountPtr<IEventQuery>;
    using FRHIResourceRef           = TRefCountPtr<IRHIResource>;
    using FRHIBufferRef             = TRefCountPtr<FRHIBuffer>;
    using FRHIImageRef              = TRefCountPtr<FRHIImage>;
    using FRHISamplerRef            = TRefCountPtr<FRHISampler>;
    using FRHIShaderRef             = TRefCountPtr<FRHIShader>;
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
    using FRHIShaderLibraryRef      = TRefCountPtr<FShaderLibrary>;
    using FRHIDescriptorTableRef    = TRefCountPtr<FRHIDescriptorTable>;

}
