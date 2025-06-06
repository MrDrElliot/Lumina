﻿#pragma once

#include "Module/API.h"
#include "ObjectFlags.h"
#include "Core/LuminaMacros.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class CObjectBase;
    class CEnum;
    class CObject;
    class CClass;
}

#define TRANSIENT_PACKAGE TEXT("Transient")

namespace Lumina
{
    extern THashMap<FName, CObjectBase*> ObjectNameHash;
    
    struct FConstructCObjectParams
    {
        
        FConstructCObjectParams(const CClass* InClass)
            : Class(InClass)
            , Name()
            , Package(nullptr)
            , Flags(EObjectFlags::OF_None)
        {}
        
        const CClass* Class;
        FName Name;
        const TCHAR* Package;
        EObjectFlags Flags;
    };

    LUMINA_API FName MakeUniqueObjectName(const CClass* Class, FName InBaseName = NAME_None);
    LUMINA_API CObject* StaticAllocateObject(const FConstructCObjectParams& Params);

    LUMINA_API CObject* FindObjectFast(const CClass* InClass, FName QualifiedName);
    LUMINA_API CObject* StaticLoadObject(const CClass* InClass, const TCHAR* QualifiedName);
    
    LUMINA_API void ResolveObjectPath(FString& OutPath, const FStringView& InPath);
    
    template<typename T>
    inline T* FindObject(const TCHAR* QualifiedName)
    {
        return (T*)FindObjectFast(T::StaticClass(), QualifiedName);
    }

    template<typename T>
    inline T* LoadObject(const TCHAR* FullName)
    {
        return (T*)StaticLoadObject(T::StaticClass(), FullName);
    }

    

    template<typename T>
    T* NewObject(const TCHAR* Package = TRANSIENT_PACKAGE, FName Name = NAME_None, EObjectFlags Flags = OF_None)
    {
        FConstructCObjectParams Params(T::StaticClass());
        Params.Name = Name;
        Params.Flags = Flags;
        Params.Package = Package;

        CObject* Obj = StaticAllocateObject(Params);
        
        return (T*)Obj;
        
    }

    template<typename T>
    T* NewObject(CClass* InClass, const TCHAR* Package = TRANSIENT_PACKAGE, FName Name = NAME_None, EObjectFlags Flags = OF_None)
    {
        FConstructCObjectParams Params(InClass);
        Params.Name = Name;
        Params.Flags = Flags;
        Params.Package = Package;

        CObject* Obj = StaticAllocateObject(Params);
        
        return (T*)Obj;
        
    }

    template<typename T>
    T* GetMutableDefault()
    {
        return T::StaticClass()->template GetDefaultObject<T>();
    }

    template<typename T>
    const T* GetDefault()
    {
        return T::StaticClass()->template GetDefaultObject<T>();
    }
    
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
    using FEnumRegistrationInfo = TRegistrationInfo<CEnum>;


    struct FPropertyParams
    {
        const char*         Name;
        EPropertyFlags      PropertyFlags;
        EPropertyTypeFlags  TypeFlags;
        uint16              Offset;
    };
    
    struct FClassParams
    {
        CClass*                         (*RegisterFunc)();
        const FPropertyParams* const*   Params;
        uint32                          NumProperties;
    };

    
    struct FEnumeratorParam
    {
        const char*               NameUTF8;
        int64                     Value;
    };
    
    struct FEnumParams
    {
        const char*                 Name;
        const FEnumeratorParam*     Params;
        int16                       NumParams;
    };

    

    LUMINA_API void ConstructCClass(CClass** OutClass, const FClassParams& Params);
    LUMINA_API void ConstructCEnum(CEnum** OutEnum, const FEnumParams& Params);
    
    
}
