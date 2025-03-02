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
        TRefCountPtr<IImageState> ImageState = GetImageAccess(Image);

        if(ImageState->State == ERHIAccess::None)
        {
            //LOG_WARN("Image being treated as undefined due to having no initial RHI access!");
        }

        if(ImageState->State != Access)
        {
            ImageBarriers.emplace_back(Image, ImageState->State, Access);
        }

        ImageState->State = Access;
    }

    TRefCountPtr<IImageState> FCommandListStateTracker::GetImageAccess(FRHIImageRef Image)
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

        auto NewState = MakeRefCount<IImageState>(DefaultState);
        ImageStates.emplace(Image, NewState);

        return NewState;
    }


    void FCommandListStateTracker::CommandListExecuted(ICommandList* CommandList)
    {
        Assert(ImageBarriers.empty());
        Assert(BufferBarriers.empty());
        
        ImageStates.clear();
        BufferStates.clear();
        
    }

    void FCommandListStateTracker::ResetImageDefaultStates()
    {
        for (auto& KVP : ImageStates)
        {
            if(KVP.first->GetDefaultState() != ERHIAccess::None && KVP.second->State != KVP.first->GetDefaultState())
            {
                RequireImageAccess(KVP.first, KVP.first->GetDefaultState());
            }
        }
    }

    void FCommandListStateTracker::ResetBufferDefaultStates()
    {
        
    }
}
