#pragma once
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    class FSubsystemManager;
}

namespace Lumina
{
    class IEditorToolContext
    {
    public:

        IEditorToolContext() = default;

        FORCEINLINE const FSubsystemManager* GetSubsystemManager() const { return SubsystemManager; }
        

    public:

        const FSubsystemManager*      SubsystemManager = nullptr;
    };
}
