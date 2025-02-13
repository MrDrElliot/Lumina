#include "Thread.h"


namespace Lumina
{
    namespace Threading
    {

        static std::thread::id gMainThreadID = {};
        
        bool IsMainThread()
        {
            return gMainThreadID == std::this_thread::get_id();
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