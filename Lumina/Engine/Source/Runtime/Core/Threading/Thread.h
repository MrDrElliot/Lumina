#pragma once
#include <mutex>


namespace Lumina
{
    using FThread =             std::thread;
    using FMutex =              std::mutex;
    using FRecursiveMutex =     std::recursive_mutex;
    using FScopeLock =          std::lock_guard<FMutex>;

    // ------------------------------------------------------------------------------
    
    using FRecursiveScopeLock = std::lock_guard<FRecursiveMutex>;

}
