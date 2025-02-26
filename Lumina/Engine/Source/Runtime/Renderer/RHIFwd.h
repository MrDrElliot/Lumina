#pragma once

#include "RenderHandle.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
    
    //----------------------------------------------------------------------------

    using FRHIImageHandle                   = TRenderResourceHandle<ERenderHandleType::Image>;
    using FRHIBufferHandle                  = TRenderResourceHandle<ERenderHandleType::Buffer>;
    using FRHIShaderHandle                  = TRenderResourceHandle<ERenderHandleType::Shader>;
    using FRHIGraphicsPipelineHandle        = TRenderResourceHandle<ERenderHandleType::GraphicsPipeline>;
    using FRHIComputePipelineHandle        = TRenderResourceHandle<ERenderHandleType::ComputePipeline>;

}
