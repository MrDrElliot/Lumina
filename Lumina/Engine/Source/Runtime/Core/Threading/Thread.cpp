#include "Thread.h"

#include "rpmalloc.h"
#include "Core/Assertions/Assert.h"


namespace Lumina
{
    namespace Threading
    {

        static std::thread::id gMainThreadID = {};
        static std::thread::id gRenderThreadID = {};

        uint64 GetThreadID()
        {
            return std::this_thread::get_id()._Get_underlying_id();
        }

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

        void Sleep(uint64 Miliseconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Miliseconds));
        }

        void Initialize(const char* MainThreadName)
        {
            gMainThreadID = std::this_thread::get_id();
        }

        void Shutdown()
        {
            gMainThreadID = {};
        }

        void InitializeThreadHeap()
        {
            rpmalloc_thread_initialize();
        }

        void ShutdownThreadHeap()
        {
            rpmalloc_thread_finalize(1);
        }
    }
}
