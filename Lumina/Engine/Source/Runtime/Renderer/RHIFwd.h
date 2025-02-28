#pragma once

#include "RHIFwd.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
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

}
