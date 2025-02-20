#pragma once

#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Renderer/RenderResource.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FBuffer;
    class FDescriptorSet;
    class FShader;
    

    class FPipeline : public IRenderResource
    {
    public:

        using SetIndex = uint32;
        
        FPipeline() = default;
        virtual ~FPipeline() = default;
        
        static FRHIPipeline Create(const FPipelineSpec& PipelineSpec);

        FORCEINLINE const FPipelineSpec& GetSpecification() const { return Specification; }

        virtual void* GetPlatformPipeline() const = 0;
        virtual void* GetPlatformPipelineLayout() const = 0;
        
        virtual void CreateGraphics() = 0;
        virtual void CreateCompute() = 0;

    protected:

        FPipelineSpec                  Specification = {};
    };
}
