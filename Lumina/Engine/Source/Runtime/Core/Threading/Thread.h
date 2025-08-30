#pragma once

#include <mutex>
#include "Module/API.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    using FThread =             std::thread;
    using FMutex =              std::mutex;
    using FRecursiveMutex =     std::recursive_mutex;
    using FScopeLock =          std::scoped_lock<FMutex>;
    using FRecursiveScopeLock = std::lock_guard<FRecursiveMutex>;

    namespace Threading
    {

        enum class ENamedThreads : uint8
        {
            MainThread,
            RenderThread,
        };
        

        LUMINA_API uint64 GetThreadID();
        LUMINA_API bool IsMainThread();
        LUMINA_API bool IsRenderThread();

        LUMINA_API uint32 GetNumThreads();

        LUMINA_API void SetRenderThread(std::thread::id ID);

        LUMINA_API void Sleep(uint64 Milliseconds);
        
        LUMINA_API void Initialize(const char* MainThreadName);
        LUMINA_API void Shutdown();

        LUMINA_API void InitializeThreadHeap();
        LUMINA_API void ShutdownThreadHeap();
        LUMINA_API bool SetThreadName(const char* Name);
    }
    

}
