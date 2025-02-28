#pragma once

#include "RHIFwd.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
    class FRHIBuffer;
    class FRHIImage;
    
    //----------------------------------------------------------------------------

    using FRHIBufferRef             = TRefCountPtr<FRHIBuffer>;
    using FRHIImageRef              = TRefCountPtr<FRHIImage>;
    

}
