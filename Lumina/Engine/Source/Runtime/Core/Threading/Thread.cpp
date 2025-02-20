#include "Thread.h"

#include "Core/Assertions/Assert.h"


namespace Lumina
{
    namespace Threading
    {

        static std::thread::id gMainThreadID = {};
        static std::thread::id gRenderThreadID = {};
        
        bool IsMainThread()
        {
            return gMainThreadID == std::this_thread::get_id();
        }

        bool IsRenderThread()
        {
            return gRenderThreadID == std::this_thread::get_id();
        }

        uint32 GetNumThreads()
        {
            return std::thread::hardware_concurrency();
        }

        void SetRenderThread(std::thread::id ID)
        {
            gRenderThreadID = ID;
            Assert(gRenderThreadID != gMainThreadID);
        }

        void Initialize(const char* MainThreadName)
        {
            gMainThreadID = std::this_thread::get_id();
        }

        void Shutdown()
        {
            gMainThreadID = {};
        }
    }
}
