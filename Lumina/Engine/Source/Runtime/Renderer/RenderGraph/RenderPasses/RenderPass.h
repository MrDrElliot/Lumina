#pragma once

#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "Renderer/RHIFwd.h"
#include "Renderer/RenderGraph/RenderGraph.h"

namespace Lumina
{
    class IRenderContext;
    class FRenderGraphBuilder;
    class FRenderGraphScope;
}

namespace Lumina
{
    enum class ERenderPassAccess : uint8
    {
        Read,
        Write,
    };
    
    class IRenderPass
    {
        friend class FRenderGraph;
    public:
        
        virtual ~IRenderPass() = default;
        
        FName GetPassName() const { return PassName; }

        virtual void Execute(FRenderGraphScope& Scope) = 0;

    private:
        
        FName PassName;
        TFunction<void(FRenderGraphBuilder&)> BuildFunctor;
    };
}
