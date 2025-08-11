#pragma once

#include "RenderGraphPass.h"
#include "Memory/Allocators/Allocator.h"

namespace Lumina
{
    template <ERGPassFlags PassFlags, ExecutorConcept ExecutorType>
    FRGPassHandle FRenderGraph::AddPass(FRGEvent&& Event, const FRGPassDescriptor* Parameters, ExecutorType&& Executor)
    {
        return AddPassInternal(std::move(Event), Parameters, PassFlags, std::forward<ExecutorType>(Executor));
    }

    template<typename ExecutorType>
    FRGPassHandle FRenderGraph::AddPassInternal(FRGEvent&& Event, const FRGPassDescriptor* Parameters, ERGPassFlags Flags, ExecutorType&& Executor)
    {
        FRGPassHandle Pass =  GraphAllocator.TAlloc<TRGPass<ExecutorType>>(std::move(Event), Flags, Parameters, std::forward<ExecutorType>(Executor));

        Passes.push_back(Pass);
        
        return Pass;
    }

    template<typename... TArgs>
    FRGPassDescriptor* FRenderGraph::AllocParams(TArgs&&... Args)
    {
        return GraphAllocator.TAlloc<FRGPassDescriptor>(std::forward<TArgs>(Args)...);
    }
}