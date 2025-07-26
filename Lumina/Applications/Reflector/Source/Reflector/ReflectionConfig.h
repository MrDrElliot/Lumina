#pragma once
#include <cstdint>


namespace Lumina::Reflection
{

    enum class EReflectionMacro : uint8_t
    {
        Field,
        Method,
        Class,
        Struct,
        Enum,
        GeneratedBody,
        Size,
    };

    inline const char* ReflectionEnumToString(EReflectionMacro Macro)
    {
        switch (Macro)
        {
            case EReflectionMacro::Field:               return "LUM_PROPERTY";
            case EReflectionMacro::Method:              return "LUM_FUNCTION";
            case EReflectionMacro::Class:               return "LUM_CLASS";
            case EReflectionMacro::Struct:              return "LUM_STRUCT";
            case EReflectionMacro::Enum:                return "LUM_ENUM";
            case EReflectionMacro::GeneratedBody:       return "GENERATED_BODY";
            default:                                    return "NONE";
        }
        
        return nullptr;
    }


    constexpr static const char* GEngineNamespace = "Lumina";
    
    
}
