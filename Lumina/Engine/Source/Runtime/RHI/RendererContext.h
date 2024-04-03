#pragma once
#include "Source/Runtime/CoreObject/Object.h"

namespace Lumina
{
    class LRendererContext : public LObject
    {
    public:

        LRendererContext() = default;
        virtual ~LRendererContext() = default;

        virtual void Init() = 0;

        static LRendererContext* Create();
        
    };
}
