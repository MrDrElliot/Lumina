#pragma once

#include "Macros.h"
#include "Core/Object/ObjectMacros.h"

namespace Lumina
{
    LUM_ENUM()
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

        ETestEnum Test;
    
    };

    
    LUM_STRUCT()
    struct TestStruct
    {

        uint32 TestInt;
        float TestFloat;
        double TestDouble;
    };
}
