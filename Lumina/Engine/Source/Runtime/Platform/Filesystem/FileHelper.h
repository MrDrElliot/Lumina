#pragma once
#include <fstream>

#include "Containers/Array.h"

namespace Lumina
{
    struct FFileHelper 
    {
    
        static bool SaveArrayToFile(const TVector<uint8>& Array, const std::filesystem::path& Path, uint32 WriteFlags = 0);

        static bool LoadFileToArray(TVector<uint8>& Result, const std::filesystem::path& Path, uint32 ReadFlags = 0);
    
    };
}
