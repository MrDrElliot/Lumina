#pragma once
#include "Platform/GenericPlatform.h"


namespace Lumina::Reflection
{

    enum class EReflectionMacro : uint8
    {
        Field,
        Method,
        Class,
        Struct,
        Enum,
    };

    inline const char* ReflectionEnumToString(EReflectionMacro Macro)
    {
        switch (Macro)
        {
            case EReflectionMacro::Field: return "LUM_FIELD";
            case EReflectionMacro::Method: return "LUM_METHOD";
            case EReflectionMacro::Class: return "LUM_CLASS";
            case EReflectionMacro::Struct: return "LUM_STRUCT";
            case EReflectionMacro::Enum: return "LUM_ENUM";
        }
        
        return nullptr;
    }


    constexpr static const char* GEngineNamespace = "Lumina";
    
    
}
