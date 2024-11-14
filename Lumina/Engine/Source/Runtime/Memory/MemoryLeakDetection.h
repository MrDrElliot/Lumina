#pragma once
#include "Memory.h"
#include "Core/Singleton/Singleton.h"
#include "Log/Log.h"
#include "Platform/GenericPlatform.h"

namespace Lumina::Memory
{
    class MemoryLeakDetector : public TSingleton<MemoryLeakDetector>
    {
    public:

        void Shutdown() override {}

        static void PreFrame()
        {
            Get()->PreFrameMemory = gProgramMemory;    
        }
        
        static void PostFrame()
        {
            //@TODO no worky worky
            //Get()->DetectLeak(Get()->PreFrameMemory, gProgramMemory);
        }

        void DetectLeak(uint64 pre, uint64 post)
        {
            if(post > pre)
            {
                LOG_CRITICAL("Memory Leak Detected! Pre-Frame Memory: {0} | Post-Frame Memory: {1}", pre, post);
                LOG_CRITICAL("Total Number of Allocations: {0}", gTotalAllocations);
                PrintCallStack();
            }
        }


    private:

        uint64 PreFrameMemory = 0;
        
    };
}
