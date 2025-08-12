#pragma once
#include "Module/API.h"

namespace Lumina
{
    class IRenderContext;
    
    struct FRHIGlobals
    {
        bool bRHIInitialized = false;
    };


    LUMINA_API extern FRHIGlobals GRHIGlobals;
    LUMINA_API extern IRenderContext* GRenderContext;

#define GIsRHIInitialized GRHIGlobals.bRHIInitialized

}
