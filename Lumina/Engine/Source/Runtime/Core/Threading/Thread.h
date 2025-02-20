#pragma once
#include <mutex>

#include "Platform/GenericPlatform.h"


namespace Lumina
{
    using FThread =             std::thread;
    using FMutex =              std::mutex;
    using FRecursiveMutex =     std::recursive_mutex;
    using FScopeLock =          std::lock_guard<FMutex>;

    // ------------------------------------------------------------------------------
    
    using FRecursiveScopeLock = std::lock_guard<FRecursiveMutex>;


    namespace Threading
    {

        enum class ENamedThreads : uint8
        {
            MainThread,
            RenderThread,
        };
        

        bool IsMainThread();
        bool IsRenderThread();

        uint32 GetNumThreads();

        void SetRenderThread(std::thread::id ID);
        
        void Initialize(const char* MainThreadName);
        void Shutdown();
    }
    

}
