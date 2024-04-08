#pragma once
#include "Thread.h"

namespace Lumina
{
    class FGameThread : public FThread
    {
    public:

        FGameThread() : FThread("Game Thread") {};
    
    };
}
