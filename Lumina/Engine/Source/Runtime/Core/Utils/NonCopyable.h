#pragma once

#include "Module/API.h"

namespace Lumina
{

    class LUMINA_API INonCopyable
    {
    public:
        INonCopyable() = default;
        INonCopyable(const INonCopyable&) = delete;
        INonCopyable& operator = (const INonCopyable&) = delete;
        
    };
    
}