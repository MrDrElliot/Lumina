#pragma once
#include "Renderer/RenderGraph/RenderPasses/RenderPass.h"

namespace Lumina
{
    class FSkyboxRenderPass : public IRenderPass
    {
    public:

        FSkyboxRenderPass(const FName& Name)
            :IRenderPass(Name)
        {}

        void Execute(ICommandList* CommandList) override;
    
    };
}
