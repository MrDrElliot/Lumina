#pragma once
#include "RenderGraphEvent.h"
#include "RenderGraphTypes.h"
#include "Renderer/RHIFwd.h"


namespace Lumina
{
    class FRGPassDescriptor;
    enum class ERGPassFlags : uint16;
    enum class ERHIPipeline : uint8;
}

namespace Lumina
{
    class LUMINA_API FRenderGraphPass
    {
    public:

        FRenderGraphPass(FRGEvent&& InEvent, ERGPassFlags Flags)
            : Event(std::move(InEvent))
            , PipelineType(Flags == ERGPassFlags::Compute ? EPipelineType::Compute : EPipelineType::Graphics)
        {}
        

        virtual void Execute(ICommandList& CommandList) = 0;
        EPipelineType GetPipelineType() const { return PipelineType; }
    
    protected:
        
        FRGEvent Event;
        EPipelineType PipelineType;
    };



    template <typename ExecutorType>
    requires(sizeof(ExecutorType) <= 1024)
    class TRGPass : public FRenderGraphPass
    {
    public:
        
        TRGPass(FRGEvent&& InEvent, ERGPassFlags Flags, const FRGPassDescriptor* InParams, ExecutorType&& Executor)
            : FRenderGraphPass(std::move(InEvent), Flags)
            , ExecutionLambda(std::move(Executor))
            , Parameters(InParams)
        {}


        void Execute(ICommandList& CommandList) override
        {
            ExecutionLambda(CommandList);
        }


    private:
        
        ExecutorType ExecutionLambda;
        const FRGPassDescriptor* Parameters;
    };
}
