﻿#include "TaskSystem.h"

#include "Memory/Memory.h"
#include "Memory/MemoryLeakDetection.h"

namespace Lumina
{

    static void OnStartThread(uint32 threadNum)
    {
        TracyCSetThreadName(eastl::to_string(threadNum).c_str())
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
        enki::TaskSchedulerConfig config;
        config.numTaskThreadsToCreate = std::thread::hardware_concurrency();
        config.customAllocator.alloc = CustomAllocFunc;
        config.customAllocator.free = CustomFreeFunc;
        config.profilerCallbacks.threadStart = OnStartThread;
        config.profilerCallbacks.threadStop = OnStopThread;

        Scheduler.Initialize(config);
        bInitialized = true;
    }

    void FTaskSystem::Shutdown()
    {
        Scheduler.WaitforAllAndShutdown();
        bInitialized = false;
    }
}
