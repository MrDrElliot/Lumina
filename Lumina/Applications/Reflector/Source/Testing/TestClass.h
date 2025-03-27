#pragma once
#include "Macros.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{

    enum class ETestEnum : uint8
    {
        TestOne,
        TestTwo,
        TestThree,
    };
    
    LUM_CLASS()
    class TestClass
    {
    public:


        LUM_FIELD()
        uint32 Integer = 42;
    
    };
}
