#pragma once
#include "Thread.h"

namespace Lumina
{
    class FRenderThread : public FThread
    {
    public:

        FRenderThread() : FThread("Render Thread") {};
    
    };
}
