#pragma once
#include "Platform/GenericPlatform.h"
#include "Containers/String.h"
#include "Containers/Array.h"


namespace Lumina::Platform
{
    void* GetDLLHandle(const TCHAR* Filename);
    bool FreeDLLHandle(void* DLLHandle);
    void* GetDLLExport(void* DLLHandle, const TCHAR* ProcName);
    void AddDLLDirectory(const FString& Directory);

    void PushDLLDirectory(const TCHAR* Directory);
    void PopDLLDirectory();

    uint32 GetCurrentProcessID();
    uint32 GetCurrentCoreNumber();

    const TCHAR* ExecutableName(bool bRemoveExtension = true);

    LUMINA_API SIZE_T GetProcessMemoryUsageBytes();
    
    const TCHAR* BaseDir();

    void* LoadLibraryWithSearchPaths(const FString& Filename, const TVector<FString>& SearchPaths);
}
