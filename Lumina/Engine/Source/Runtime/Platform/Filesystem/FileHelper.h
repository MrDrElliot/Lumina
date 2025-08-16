#pragma once


#include "Module/API.h"
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina::FileHelper
{

    LUMINA_API bool SaveArrayToFile(const TVector<uint8>& Array, const FString& Path, uint32 WriteFlags = 0);
    LUMINA_API bool LoadFileToArray(TVector<uint8>& Result, const FString& Path, uint32 ReadFlags = 0);
    LUMINA_API bool LoadFileToArray(TVector<uint8>& Result, const FString& Path, uint32 Seek, uint32 ReadSize, uint32 ReadFlags = 0);
    LUMINA_API FString FileFinder(const FString& FileName, const FString& IteratorPath, bool bRecursive = true);
    
    LUMINA_API bool LoadFileIntoString(FString& OutString, const FString& Path, uint32 ReadFlags = 0);
    LUMINA_API bool SaveStringToFile(const FStringView& String, const FString& Path, uint32 WriteFlags = 0);
    LUMINA_API bool DoesFileExist(const FString& FilePath);
    LUMINA_API bool CreateNewFile(const FString& FilePath, bool bBinary = false, uint32 Flags = 0);
    LUMINA_API uint64 GetFileSize(const FString& FilePath);
    
}
