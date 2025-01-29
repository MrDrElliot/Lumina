#pragma once
#include <crtdbg.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "Containers/String.h"
#include "Platform/GenericPlatform.h"

inline uint64 gProgramMemory = 0;
inline uint64 gTotalAllocations = 0;


// Custom global new operator
inline void* operator new(size_t size)
{      
    gTotalAllocations++;
    gProgramMemory += size;
    return malloc(size);
}

// Custom global delete operator
inline void operator delete(void* ptr, size_t size) noexcept
{
    gTotalAllocations--;
    gProgramMemory -= size;
    free(ptr);
}

inline void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
    return new uint8_t[size];
}

namespace Lumina::Memory
{
    inline FString GetProgramMemoryAsString()
    {
        const double bytes = static_cast<double>(gProgramMemory);
        const double kibibytes = bytes / 1024.0;
        const double mebibytes = kibibytes / 1024.0;
        const double gibibytes = mebibytes / 1024.0;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(4);
        ss << "Dumping Program Memory: \n"
        << "Total Allocations: " << gTotalAllocations <<  "\n" 
        << "------- " << kibibytes << " KiB, \n" 
        << "------- " <<  mebibytes << " MiB, \n" 
        << "------- " <<  gibibytes << " GiB  \n"
        << "End Program Memory Dump";

        return FString(ss.str().c_str());
    }
    

    // Function to format size in different units
    inline FString FormatSize(uint64 bytes)
    {
        const uint64 KiB = 1024;
        const uint64 MiB = KiB * 1024;
        const uint64 GiB = MiB * 1024;

        std::ostringstream formattedSize;
        formattedSize << bytes << " bytes";

        if (bytes >= GiB)
        {
            formattedSize << " (" << std::fixed << std::setprecision(2) << (bytes / GiB) << " GiB)";
        } else if (bytes >= MiB)
        {
            formattedSize << " (" << std::fixed << std::setprecision(2) << (bytes / MiB) << " MiB)";
        } else if (bytes >= KiB)
        {
            formattedSize << " (" << std::fixed << std::setprecision(2) << (bytes / KiB) << " KiB)";
        }

        return formattedSize.str().c_str();
    }
}