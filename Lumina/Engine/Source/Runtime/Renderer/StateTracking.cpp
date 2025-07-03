#include "StateTracking.h"

#include "RenderResource.h"

namespace Lumina
{
    void FCommandListStateTracker::ClearBarriers()
    {
        ImageBarriers.clear();
        BufferBarriers.clear();
    }

    void FCommandListStateTracker::RequireImageAccess(FRHIImageRef Image, ERHIAccess Access)
    {
        IImageState ImageState = GetImageAccess(Image);

        if(ImageState.State != Access)
        {
            ImageBarriers.emplace_back(Image, ImageState.State, Access);
        }

        ImageStates[Image].State = Access;
    }

    void FCommandListStateTracker::RequireBufferAccess(FRHIBufferRef Buffer, ERHIAccess Access)
    {
        IBufferState BufferState = GetBufferAccess(Buffer);
        
        if(BufferState.State != Access)
        {
            BufferBarriers.emplace_back(Buffer, BufferState.State, Access);
        }

        BufferStates[Buffer].State = Access;
    }

    IImageState FCommandListStateTracker::GetImageAccess(FRHIImageRef Image)
    {
        auto it = ImageStates.find(Image);
        if (it != ImageStates.end())
        {
            return it->second;
        }

        ERHIAccess InitialState = Image->GetInitialAccess();
        ERHIAccess DefaultState = Image->GetDefaultState();

        if ((!Image->IsStateInitialized()) && InitialState != DefaultState)
        {
            ImageBarriers.emplace_back(Image, InitialState, DefaultState);
            Image->SetStateInitialized();
        }

        auto State = IImageState();
        State.State = DefaultState;
        
        ImageStates.insert_or_assign(Image, State);

        return ImageStates[Image];
        
    }

    IBufferState FCommandListStateTracker::GetBufferAccess(FRHIBufferRef Buffer)
    {
        auto it = BufferStates.find(Buffer);
        if (it != BufferStates.end())
        {
            return it->second;
        }

        ERHIAccess InitialState = Buffer->GetInitialAccess();
        ERHIAccess DefaultState = Buffer->GetDefaultState();

        if ((!Buffer->IsStateInitialized()) && InitialState != DefaultState)
        {
            BufferBarriers.emplace_back(Buffer, InitialState, DefaultState);
            Buffer->SetStateInitialized();
        }

        auto State = IBufferState();
        State.State = DefaultState;
        BufferStates.insert_or_assign(Buffer, State);

        return BufferStates[Buffer];
    }

    void FCommandListStateTracker::CommandListExecuted(ICommandList* CommandList)
    {
        Assert(ImageBarriers.empty())
        Assert(BufferBarriers.empty())
        
        
    }

    void FCommandListStateTracker::ResetImageDefaultStates()
    {
        for (const auto& KVP : ImageStates)
        {
            if(KVP.first->GetDefaultState() != ERHIAccess::None && KVP.second.State != KVP.first->GetDefaultState())
            {
                RequireImageAccess(KVP.first, KVP.first->GetDefaultState());
            }
        }
    }

    void FCommandListStateTracker::ResetBufferDefaultStates()
    {
        for (const auto& KVP : BufferStates)
        {
            if(KVP.first->GetDefaultState() != ERHIAccess::None && KVP.second.State != KVP.first->GetDefaultState())
            {
                RequireBufferAccess(KVP.first, KVP.first->GetDefaultState());
            }
        }
    }
}
