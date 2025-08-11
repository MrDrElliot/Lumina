#pragma once
#include "Module/API.h"
#include "Platform/GenericPlatform.h"
#include "Renderer/RenderResource.h"


namespace Lumina
{
    class FRenderGraphPass;
}

namespace Lumina
{

    using FRGPassHandle = FRenderGraphPass*;
    
    enum class LUMINA_API ERGPassFlags : uint16
    {
        None = 0,

        Raster = 1 << 0,

        Compute = 1 << 1,

        Transfer = 1 << 2,
        
    };

    #define RG_Raster   ERGPassFlags::Raster
    #define RG_Compute  ERGPassFlags::Compute
    #define RG_Transfer ERGPassFlags::Transfer
    
}
