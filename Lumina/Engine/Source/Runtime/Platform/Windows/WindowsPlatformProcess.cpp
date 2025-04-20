#ifdef LE_PLATFORM_WINDOWS
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Paths/Paths.h"
#include "Platform/Process/PlatformProcess.h"
#include <Windows.h>
#include <tchar.h>
#include <PathCch.h>  // For PathFindFileName
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "PathCch.lib")

namespace Lumina::Platform
{
    namespace
    {
        static TVector<FString> GDLLSearchPaths;
    }
    
    void* GetDLLHandle(const TCHAR* Filename)
    {
        FWString WideString = Filename;
        TVector<FString> SearchPaths = GDLLSearchPaths;

        
        return LoadLibraryWithSearchPaths(StringUtils::FromWideString(WideString), SearchPaths);
    }

    bool FreeDLLHandle(void* DLLHandle)
    {
        return FreeLibrary((HMODULE)DLLHandle);
    }

    void* GetDLLExport(void* DLLHandle, const TCHAR* ProcName)
    {
        FWString WideString = ProcName;

        return (void*)::GetProcAddress((HMODULE)DLLHandle, StringUtils::FromWideString(WideString).c_str());
    }

    void AddDLLDirectory(const FString& Directory)
    {
        GDLLSearchPaths.push_back(Directory);
    }

    void PushDLLDirectory(const TCHAR* Directory)
    {
        SetDllDirectory(Directory);
        
        GDLLSearchPaths.push_back(StringUtils::FromWideString(Directory));

        LOG_WARN("Pushing DLL Search Path: {0}", StringUtils::FromWideString(Directory));
    }

    void PopDLLDirectory()
    {
        GDLLSearchPaths.pop_back();

        if (GDLLSearchPaths.empty())
        {
            SetDllDirectory(L"");
        }
        else
        {
            SetDllDirectory(StringUtils::ToWideString(GDLLSearchPaths.back()).c_str());
        }
    }

    uint32 GetCurrentProcessID()
    {
        return 0;
    }

    uint32 GetCurrentCoreNumber()
    {
        return 0;
    }
    

    const TCHAR* ExecutableName(bool bRemoveExtension)
    {
        static TCHAR ExecutablePath[MAX_PATH];
    
        if (GetModuleFileName(NULL, ExecutablePath, MAX_PATH) == 0)
        {
            return nullptr;
        }

        TCHAR* ExecutableName = PathFindFileName(ExecutablePath);

        // If bRemoveExtension is true, remove the file extension
        if (bRemoveExtension)
        {
            PathCchRemoveExtension(ExecutableName, MAX_PATH);
        }

        return ExecutableName;
    }


    const TCHAR* BaseDir()
    {
        static TCHAR Buffer[MAX_PATH] = {};
        if (Buffer[0] == 0)
        {
            GetModuleFileNameW(nullptr, Buffer, MAX_PATH);
        }
        return Buffer;
    }

    void* LoadLibraryWithSearchPaths(const FString& Filename, const TVector<FString>& SearchPaths)
    {
        if (Paths::Exists(Filename))
        {
            FWString Wide = StringUtils::ToWideString(Filename);
            if (void* Handle = GetModuleHandleW(Wide.c_str()))
            {
                return Handle;
            }

            if (void* Handle = LoadLibraryW(Wide.c_str()))
            {
                return Handle;
            }
        }

        for (const FString& Path : SearchPaths)
        {
            FString FullPath = Paths::Combine(Path.c_str(), Filename.c_str());
            if (Paths::Exists(FullPath))
            {
                FWString Wide = StringUtils::ToWideString(FullPath);
                if (void* Handle = LoadLibraryW(Wide.c_str()))
                {
                    return Handle;
                }
            }
        }

        return nullptr;
    }
}


#endif