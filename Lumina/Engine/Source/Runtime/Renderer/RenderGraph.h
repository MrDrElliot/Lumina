#pragma once
#include "RenderPass.h"
#include "Containers/Name.h"

namespace Lumina
{
    
    class FRenderGraph
    {
    public:

        struct FPassContext
        {
            FRHICommandBuffer CommandBuffer;
        };

        
        void AddPass(const FName& PassName, const FRenderPassBeginInfo& PassInfo);
    
    };
}
