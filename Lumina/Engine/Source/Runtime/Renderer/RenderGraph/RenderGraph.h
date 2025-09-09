#pragma once
#include "RenderGraphEvent.h"
#include "RenderGraphResourceRegistry.h"
#include "RenderGraphResources.h"
#include "RenderGraphTypes.h"
#include "Containers/Array.h"
#include "Memory/Allocators/Allocator.h"
#include "Renderer/RHIFwd.h"


namespace Lumina
{
    class FRGPassDescriptor;
    struct FBindingLayoutDesc;
    class FRHIBindingSet;
    class FRenderGraphPass;
}



namespace Lumina
{
    template <typename ExecutorType>
    concept ExecutorConcept = (sizeof(ExecutorType) <= 1024) && std::invocable<ExecutorType, ICommandList&>;
    
    class LUMINA_API FRenderGraph
    {
    public:

        FRenderGraph();
        
        template <ERGPassFlags PassFlags, ExecutorConcept ExecutorType>
        FRGPassHandle AddPass(FRGEvent&& Event, const FRGPassDescriptor* Parameters, ExecutorType&& Executor);

        template<typename... TArgs>
        FRGPassDescriptor* AllocParams(TArgs&&... Args);
        
        void Execute();
        void Compile();

        void AllocateTransientResources();
        
        FRGBuffer* CreateBuffer(const FRHIBufferDesc& Desc);
        FRGImage* CreateImage(const FRHIImageDesc& Desc);
        
    private:
        
        template<typename ExecutorType>
        FRGPassHandle AddPassInternal(FRGEvent&& Event, const FRGPassDescriptor* Parameters, ERGPassFlags Flags, ExecutorType&& Executor);


    private:

        struct FResourceOp
        {
            enum
            {
                Alloc,
                Dealloc,
            };
        };
        

    private:

        FRHICommandListRef              GraphicsCommandList;
        FRHICommandListRef              ComputeCommandList;
        FRHICommandListRef              TransferCommandList;

        TRGHandleRegistry<FRGBuffer>    BufferRegistry;
        TRGHandleRegistry<FRGImage>     ImageRegistry;
        
        FLinearAllocator                GraphAllocator;
        TFixedVector<FRGPassHandle, 12> Passes;
    };
}

#include "RenderGraph.inl"