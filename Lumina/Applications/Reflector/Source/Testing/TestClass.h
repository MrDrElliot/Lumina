#pragma once

#include "Macros.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    ENUM()
    enum class ETestEnum : uint8
    {
        TestOne,
        TestTwo,
        TestThree,
    };

    
    CLASS()
    class TestClass
    {
    public:

        
        PROPERTY()
        uint32 Value = 42;
    
    };
}
