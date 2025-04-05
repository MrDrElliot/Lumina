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
        
        LUM_FIELD()
        ETestEnum Test;

        
    };

    
    LUM_STRUCT()
    struct TestStruct
    {

        LUM_FIELD()
        uint32 TestInt;
        
        LUM_FIELD()
        float TestFloat;

        LUM_FIELD()
        double TestDouble;
    };
}
