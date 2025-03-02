#pragma once
#include "Platform/GenericPlatform.h"
#include "Types/BitFlags.h"

namespace Lumina
{

    enum class EPendingGraphicsState : uint8
    {
        Idle = 0,
        Recording = 1 << 0,
        RenderPass = 1 << 1,
    };
    
    class FPendingGraphicsState
    {
    public:

        FORCEINLINE void AddPendingState(EPendingGraphicsState State) { PendingGraphicsState.SetFlag(State); }
        FORCEINLINE void ClearPendingState(EPendingGraphicsState State) { PendingGraphicsState.ClearFlag(State); }
        FORCEINLINE void Reset() { PendingGraphicsState.ClearAllFlags(); }
        

        FORCEINLINE bool IsRecording() const { return PendingGraphicsState.IsFlagSet(EPendingGraphicsState::Recording); }
        FORCEINLINE bool IsInRenderPass() const { return PendingGraphicsState.IsFlagSet(EPendingGraphicsState::Recording); }
        FORCEINLINE bool IsIdle() const { return PendingGraphicsState.IsFlagSet(EPendingGraphicsState::Idle); }

    private:
        
        TBitFlags<EPendingGraphicsState> PendingGraphicsState;
    
    };
}
