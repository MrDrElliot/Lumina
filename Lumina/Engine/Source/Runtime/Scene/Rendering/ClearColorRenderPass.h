#pragma once
#include "Renderer/RenderGraph/RenderPasses/RenderPass.h"

namespace Lumina
{
    class FClearColorRenderPass : public IRenderPass
    {
    public:

        void Execute(FRenderGraphScope& Scope) override;
    };
}
