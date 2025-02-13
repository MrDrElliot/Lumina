#include "ApplicationGlobalState.h"

#include "Containers/Name.h"
#include "Core/Threading/Thread.h"
#include "Log/Log.h"
#include "Memory/Memory.h"

namespace Lumina
{
    namespace
    {
        static bool bGlobalStateInitialize = false;
    }
    
    FApplicationGlobalState::FApplicationGlobalState(char const* MainThreadName)
    {
        Assert(!bGlobalStateInitialize);
        
        FMemory::Initialize();
        Threading::Initialize(MainThreadName == nullptr ? "Main Thread" : MainThreadName);
        FName::Initialize();
        FLog::Init();

        bGlobalStateInitialize = true;
    }

    FApplicationGlobalState::~FApplicationGlobalState()
    {
        Assert(bGlobalStateInitialize);
        bGlobalStateInitialize = false;

        FLog::Shutdown();
        FName::Shutdown();
        Threading::Shutdown();
        FMemory::Shutdown();
    }
}
