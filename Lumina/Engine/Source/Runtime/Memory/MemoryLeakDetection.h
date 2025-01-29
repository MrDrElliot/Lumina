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
        }
        
        static void PostFrame()
        {
            //@TODO no worky worky
        }

        void DetectLeak(uint64 pre, uint64 post)
        {
            if(post > pre)
            {
                PrintCallStack();
            }
        }


    private:

        uint64 PreFrameMemory = 0;
        
    };
}
