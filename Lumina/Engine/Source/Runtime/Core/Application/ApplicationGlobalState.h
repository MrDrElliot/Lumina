#pragma once

#include "Module/API.h"

namespace Lumina
{
    
    
    class LUMINA_API FApplicationGlobalState
    {
    public:

        FApplicationGlobalState(char const* MainThreadName = nullptr);
        ~FApplicationGlobalState();
        
    };
}