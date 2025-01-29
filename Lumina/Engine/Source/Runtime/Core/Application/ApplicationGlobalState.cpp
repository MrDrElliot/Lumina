#include "ApplicationGlobalState.h"

#include "Log/Log.h"

namespace Lumina
{
    FApplicationGlobalState::FApplicationGlobalState(char const* MainThreadName)
    {
        FLog::Init();
    }

    FApplicationGlobalState::~FApplicationGlobalState()
    {
        FLog::Shutdown();
    }
}
