#pragma once

#include <chrono>
#include <string>
#include <mutex>
#include <unordered_map>

#include "Core/Singleton/Singleton.h"

// FPerformanceTracker using TSingleton
class FPerformanceTracker : public TSingleton<FPerformanceTracker>
{
public:
    // Mapping profile names to durations
    std::unordered_map<std::string, double> ProfileMap;
    
    // Mutex to protect access to the map
    std::mutex ProfileMapMutex;

    void Shutdown() override {}

    // Add or update a profiling entry
    void AddProfileData(const std::string& name, double duration)
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
#else
#define PROFILE_SCOPE(name) // Do nothing in release builds
#endif