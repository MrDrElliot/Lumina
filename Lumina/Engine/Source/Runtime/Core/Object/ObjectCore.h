#pragma once
#include "Core/LuminaMacros.h"
#include "Module/API.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CObject;
    class CClass;
}

namespace Lumina
{

    enum class EPropertyFlags : uint64
    {
        None = 0,
        PF_Const,
        PF_Private,
        PF_Protected,
    };

    ENUM_CLASS_FLAGS(EPropertyFlags);

    enum class EPropertyTypeFlags : uint16_t
    {
        None = 0,

        // Signed integers
        Int8                = 1 << 0,
        Int16               = 1 << 1,
        Int32               = 1 << 2,
        Int64               = 1 << 3,

        // Unsigned integers
        UInt8               = 1 << 4,
        UInt16              = 1 << 5,
        UInt32              = 1 << 6,
        UInt64              = 1 << 7,

        // Floats
        Float               = 1 << 8,
        Double              = 1 << 9,

        // Other types
        Bool                = 1 << 10,
        Object              = 1 << 12,
        Class               = 1 << 13,
        Name                = 1 << 14,
        String              = 1 << 15,
    };

    ENUM_CLASS_FLAGS(EPropertyTypeFlags);
    
    template <typename T>
    struct TRegistrationInfo
    {
        using TType = T;

        TType* Singleton = nullptr;
    };

    using FClassRegistrationInfo = TRegistrationInfo<CClass>;


    struct FPropertyParams
    {
        const char*         Name;
        EPropertyFlags      PropertyFlags;
        EPropertyTypeFlags  TypeFlags;
        
    };
    
    struct FClassParams
    {
        CClass*                         (*RegisterFunc)();
        const FPropertyParams* const*   Params;
        uint32                          NumProperties;
    };

    

    LUMINA_API void ConstructCClass(CClass** OutClass, const FClassParams& Params);
    
    
}
