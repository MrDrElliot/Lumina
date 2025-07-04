#pragma once

#include "ConstructObjectParams.h"
#include "Module/API.h"
#include "ObjectFlags.h"
#include "Containers/Array.h"
#include "Core/LuminaMacros.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class CPackage;
    class CStruct;
    class CObjectBase;
    class CEnum;
    class CObject;
    class CClass;
}

namespace Lumina
{

    LUMINA_API FName MakeUniqueObjectName(const CClass* Class, const CPackage* Package, const FName& InBaseName = NAME_None);
    LUMINA_API CObject* StaticAllocateObject(FConstructCObjectParams& Params);

    LUMINA_API CObject* FindObjectFast(const CClass* InClass, const FName& QualifiedName);
    LUMINA_API CObject* StaticLoadObject(const CClass* InClass, const FName& QualifiedName);
    
    LUMINA_API void ResolveObjectPath(FString& OutPath, const FStringView& InPath);
    
    LUMINA_API FString GetPackageFromQualifiedObjectName(const FString& FullyQualifiedName);
    LUMINA_API FString GetObjectNameFromQualifiedName(const FString& FullyQualifiedName);
    LUMINA_API FString RemoveNumberSuffixFromObject(const FString& ObjectName);

    LUMINA_API FName MakeFullyQualifiedObjectName(const CPackage* Package, const FName& ObjectName);
    LUMINA_API void ResolveObjectName(FName& Name);

    LUMINA_API bool IsValid(CObjectBase* Obj);
    
    template<typename T>
    inline T* FindObject(const FName& QualifiedName)
    {
        return (T*)FindObjectFast(T::StaticClass(), QualifiedName);
    }

    template<typename T>
    inline T* LoadObject(const FName& QualifiedName)
    {
        return (T*)StaticLoadObject(T::StaticClass(), QualifiedName);
    }
    
    
    LUMINA_API CObject* NewObject(CClass* InClass, const CPackage* Package = nullptr, const FName& Name = NAME_None, EObjectFlags Flags = OF_None);
    LUMINA_API void GetObjectsWithPackage(CPackage* Package, TVector<CObject*>& OutObjects);
    
    template<typename T>
    T* NewObject(CPackage* Package = nullptr, FName Name = NAME_None, EObjectFlags Flags = OF_None)
    {
        return static_cast<T*>(NewObject(T::StaticClass(), Package, Name, Flags));
    }

    template<typename T>
    T* NewObject(CClass* InClass, CPackage* Package = nullptr, FName Name = NAME_None, EObjectFlags Flags = OF_None)
    {
        return static_cast<T*>(NewObject(InClass, Package, Name, Flags));
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
        PF_SubField,
    };

    ENUM_CLASS_FLAGS(EPropertyFlags);

    /** This must reflect EPropertyTypeFlags found in ReflectedType.h */
    enum class EPropertyTypeFlags : uint64_t
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
        Enum                = 1 << 16,
        Vector              = 1 << 17,
        Struct              = 1 << 18,
    };

    ENUM_CLASS_FLAGS(EPropertyTypeFlags);
    
    template <typename T>
    struct TRegistrationInfo
    {
        using TType = T;
        
        /** Is the first object to be constructed, and internally allocates the classes memory */
        TType* InnerSingleton = nullptr;

        /** After the InnerSingleton stage, this pointer is used to track and initialize the classes internal data, such as properties and function reflection */
        TType* OuterSingleton = nullptr;

    };

    using FStructRegistrationInfo = TRegistrationInfo<CStruct>;
    using FClassRegistrationInfo = TRegistrationInfo<CClass>;
    using FEnumRegistrationInfo = TRegistrationInfo<CEnum>;

    struct FMetaDataPairParam
    {
        const char* NameUTF8;
        const char* ValueUTF8;
    };

    struct FPropertyParams
    {
        const char*         Name;
        EPropertyFlags      PropertyFlags;
        EPropertyTypeFlags  TypeFlags;
        uint16              Offset;
    };

    struct FObjectPropertyParams : public FPropertyParams
    {
        CClass*            (*ClassFunc)();
    };

    struct FStructPropertyParams : public FPropertyParams
    {
        CStruct*            (*StructFunc)();
    };

    struct FEnumPropertyParams : public FPropertyParams
    {
        CEnum*              (*EnumFunc)();
    };

    struct FArrayPropertyParams : public FPropertyParams
    {
        
    };
    
    struct FClassParams
    {
        CClass*                         (*RegisterFunc)();
        const FPropertyParams* const*   Params;
        uint32                          NumProperties;
    };

    struct FStructParams
    {
        CStruct*                        (*SuperFunc)();
        const char*                     Name;
        const FPropertyParams* const*   Params;
        uint32                          NumProperties;
        uint16                          SizeOf;
        uint16                          AlignOf;
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
    LUMINA_API void ConstructCStruct(CStruct** OutStruct, const FStructParams& Params);
    
}
