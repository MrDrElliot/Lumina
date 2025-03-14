#pragma once
#include <fstream>

#include "Containers/Array.h"

namespace Lumina
{
    struct FFileHelper 
    {
    
        static bool SaveArrayToFile(const TVector<uint8>& Array, const FString& Path, uint32 WriteFlags = 0);
        static bool LoadFileToArray(TVector<uint8>& Result, const FString& Path, uint32 ReadFlags = 0);

        static bool LoadFileIntoString(FString& OutString, const FString& Path, uint32 ReadFlags = 0);
        static bool SaveStringToFile(const FStringView& String, const FString& Path, uint32 WriteFlags = 0);
        static bool DoesFileExist(const FString& FilePath);
        static bool CreateNewFile(const FString& FilePath, bool bBinary = false, uint32 Flags = 0);
        static uint64 GetFileSize(const FString& FilePath);
    
    };
}
