#include "ApplicationGlobalState.h"

#include "Log/Log.h"
#include "Memory/Memory.h"

namespace Lumina
{
    FApplicationGlobalState::FApplicationGlobalState(char const* MainThreadName)
    {
        FMemory::Initialize();
        FLog::Init();
    }

    FApplicationGlobalState::~FApplicationGlobalState()
    {
        FLog::Shutdown();
        FMemory::Shutdown();
    }
}
