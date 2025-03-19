#pragma once
#include "Core/LuminaMacros.h"
#include "Platform/GenericPlatform.h"
#include "Types/BitFlags.h"

namespace Lumina
{

    enum class EPendingCommandState : uint8
    {
        Idle = 0,
        Recording = 1,
    };
    
    ENUM_CLASS_FLAGS(EPendingCommandState);

    class FPendingCommandState
    {
    public:
        
        FORCEINLINE void AddPendingState(EPendingCommandState State) { PendingCommandState.SetFlag(State); }
        FORCEINLINE void ClearPendingState(EPendingCommandState State) { PendingCommandState.ClearFlag(State); }
        FORCEINLINE void Reset() { PendingCommandState.ClearAllFlags(); }
        

        FORCEINLINE bool IsRecording() const { return PendingCommandState.IsFlagSet(EPendingCommandState::Recording); }
        FORCEINLINE bool IsIdle() const { return PendingCommandState.IsFlagSet(EPendingCommandState::Idle); }
        
    protected:

        TBitFlags<EPendingCommandState> PendingCommandState;
    };
    
}
