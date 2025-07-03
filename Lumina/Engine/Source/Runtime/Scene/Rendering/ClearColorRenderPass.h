#pragma once
#include "Renderer/RenderGraph/RenderPasses/RenderPass.h"

namespace Lumina
{
    class FClearColorRenderPass : public IRenderPass
    {
    public:

        FClearColorRenderPass(const FName& Name)
            :IRenderPass(Name)
        {}
        
        void Execute(ICommandList* CommandList) override;
    
    };
}
