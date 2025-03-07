#pragma once

#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Types/BitFlags.h"


namespace Lumina
{
    struct FRHIImageDesc;
    struct FRHIBufferDesc;
}

namespace Lumina
{
    
    struct FImageBarrier
    {
        FRHIImageRef Image;
        ERHIAccess AccessBefore = ERHIAccess::None;
        ERHIAccess AccessAfter = ERHIAccess::None;
    };

    struct FBufferBarrier
    {
        FRHIBufferRef Buffer;
        ERHIAccess AccessBefore = ERHIAccess::None;
        ERHIAccess AccessAfter = ERHIAccess::None;
    };

    class IAccessableRHIResource
    {
    public:

        FORCEINLINE bool IsStateInitialized() const { return bStateInitialized; }
        FORCEINLINE void SetStateInitialized() { bStateInitialized = true; }

        FORCEINLINE void SetInitialAccess(ERHIAccess Access) { InitialAccess = Access; }
        FORCEINLINE ERHIAccess GetInitialAccess() const { return InitialAccess; }
        
        FORCEINLINE void SetDefaultAccess(ERHIAccess Access) { DefaultState = Access; }
        FORCEINLINE ERHIAccess GetDefaultState() const { return DefaultState; }

    private:

        bool bStateInitialized = false;
        ERHIAccess InitialAccess = ERHIAccess::None;
        ERHIAccess DefaultState = ERHIAccess::None;
    };

    struct IBufferState
    {
        bool bInitialized = false;
        ERHIAccess State = ERHIAccess::None;
    };

    struct IImageState
    {
        bool bInitialized = false;
        ERHIAccess State = ERHIAccess::None;
    };

    class FCommandListStateTracker
    {
    public:

        void ClearBarriers();

        void RequireImageAccess(FRHIImageRef Image, ERHIAccess Access);
        void RequireBufferAccess(FRHIBufferRef Buffer, ERHIAccess Access);
        
        IImageState GetImageAccess(FRHIImageRef Image);
        IBufferState GetBufferAccess(FRHIBufferRef Buffer);

        void CommandListExecuted(ICommandList* CommandList);

        void ResetImageDefaultStates();
        void ResetBufferDefaultStates();

        FORCEINLINE NODISCARD const TVector<FImageBarrier>& GetImageBarriers() const { return ImageBarriers; }
        FORCEINLINE NODISCARD const TVector<FBufferBarrier>& GetBufferBarriers() const { return BufferBarriers; }
    
    private:

        THashMap<FRHIImageRef, IImageState>   ImageStates;
        THashMap<FRHIBufferRef, IBufferState>  BufferStates;

        TVector<FImageBarrier>  ImageBarriers;
        TVector<FBufferBarrier> BufferBarriers;
    };
    
}
