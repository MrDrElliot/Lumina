#pragma once


#include "Module/API.h"
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina::FileHelper
{

    LUMINA_API bool SaveArrayToFile(const TVector<uint8>& Array, FStringView Path, uint32 WriteFlags = 0);
    LUMINA_API bool LoadFileToArray(TVector<uint8>& Result, FStringView Path, uint32 ReadFlags = 0);
    LUMINA_API bool LoadFileToArray(TVector<uint8>& Result, FStringView Path, uint32 Seek, uint32 ReadSize, uint32 ReadFlags = 0);
    LUMINA_API FString FileFinder(const FString& FileName, FStringView IteratorPath, bool bRecursive = true);
    
    LUMINA_API bool LoadFileIntoString(FString& OutString, FStringView Path, uint32 ReadFlags = 0);
    LUMINA_API bool SaveStringToFile(FStringView String, FStringView Path, uint32 WriteFlags = 0);
    LUMINA_API bool DoesFileExist(FStringView FilePath);
    LUMINA_API bool CreateNewFile(FStringView FilePath, bool bBinary = false, uint32 Flags = 0);
    LUMINA_API uint64 GetFileSize(FStringView FilePath);
    
}
