#pragma once

#include <mutex>
#include "EASTL/unordered_map.h"
#include "Containers/String.h"
#include "Core/Singleton/Singleton.h"

// FPerformanceTracker using TSingleton
class FPerformanceTracker : public TSingleton<FPerformanceTracker>
{
public:
    // Mapping profile names to durations
    eastl::unordered_map<Lumina::FString, double> ProfileMap;
    
    // Mutex to protect access to the map
    std::mutex ProfileMapMutex;
    
    // Add or update a profiling entry
    void AddProfileData(const Lumina::FString& name, double duration)
    {
#if _DEBUG
        std::lock_guard<std::mutex> lock(ProfileMapMutex);
        ProfileMap[name] = duration;  // Overwrites old data with new data each frame
#endif
    }
};

#ifdef _DEBUG  // Only enable profiling in debug builds
#define PROFILE_SCOPE(name) \
struct EndScope_##name { \
std::chrono::time_point<std::chrono::high_resolution_clock> start; \
EndScope_##name() : start(std::chrono::high_resolution_clock::now()) {} \
~EndScope_##name() { \
auto end = std::chrono::high_resolution_clock::now(); \
std::chrono::duration<double, std::milli> duration = end - start; \
FPerformanceTracker::Get()->AddProfileData(#name, duration.count()); \
} \
} endScope_##name;

#define PROFILE_SCOPE_LOG(name) \
struct EndScope { \
std::chrono::time_point<std::chrono::high_resolution_clock> start; \
EndScope() : start(std::chrono::high_resolution_clock::now()) {} \
~EndScope() { \
auto end = std::chrono::high_resolution_clock::now(); \
std::chrono::duration<double, std::milli> duration = end - start; \
LOG_TRACE("Scope Profile: {0} | {1} ms", #name, duration.count()); \
} \
}; \
EndScope endScope_##__COUNTER__;



#else
#define PROFILE_SCOPE_LOG(name)
#define PROFILE_SCOPE(name)
#endif
