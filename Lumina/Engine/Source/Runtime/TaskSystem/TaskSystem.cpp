#include "TaskSystem.h"

#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"

namespace Lumina
{

    static void OnStartThread(uint32 threadNum)
    {
        FString ThreadName = "Worker: " + eastl::to_string(threadNum);
        Threading::SetThreadName(ThreadName.c_str());
        
        Memory::InitializeThreadHeap();
    }

    static void OnStopThread( uint32_t threadNum )
    {
        Memory::ShutdownThreadHeap();
    }

    static void* CustomAllocFunc(size_t alignment, size_t size, void* userData_, const char* file_, int line_)
    {
        return Memory::Malloc(size, alignment);
    }

    static void CustomFreeFunc(void* ptr, size_t size, void* userData_, const char* file_, int line_)
    {
        Memory::Free(ptr);
    }
    
    void FTaskSystem::Initialize()
    {
        Assert(bInitialized == false)
        
        enki::TaskSchedulerConfig config;
        config.customAllocator.alloc = CustomAllocFunc;
        config.customAllocator.free = CustomFreeFunc;
        config.profilerCallbacks.threadStart = OnStartThread;
        config.profilerCallbacks.threadStop = OnStopThread;

        Scheduler.Initialize(config);
        bInitialized = true;
    }

    void FTaskSystem::Shutdown()
    {
        FScopeLock Lock(LambdaTaskMutex);
        
        Scheduler.WaitforAllAndShutdown();
        while (!LambdaTaskPool.empty())
        {
            LambdaTaskPool.pop();
        }
        
        bInitialized = false;
    }
}
