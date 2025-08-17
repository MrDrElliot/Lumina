#ifdef LE_PLATFORM_WINDOWS

#include "Thread.h"
#include <windows.h>
#include <tracy/TracyC.h>

namespace Lumina::Threading
{
    bool SetThreadName(const char* Name)
    {
        wchar_t WThreadName[255];
        auto pNativeThreadHandle = GetCurrentThread();
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Name, -1, WThreadName, 255);
        HRESULT Result = SetThreadDescription(pNativeThreadHandle, WThreadName);
        TracyCSetThreadName(Name)
        return Result != 0;
    }
}

#endif