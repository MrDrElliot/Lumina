#pragma once

#include "ConstructObjectParams.h"
#include "Lumina.h"
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
    T* FindObject(const FName& QualifiedName)
    {
        return (T*)FindObjectFast(T::StaticClass(), QualifiedName);
    }

    template<typename T>
    T* FindObject(const FName& Package, const FName& Name)
    {
        FString QualifiedName = Package.ToString() + "." + Name.ToString();
        return (T*)FindObjectFast(T::StaticClass(), FName(QualifiedName));
    }
    
    template<typename T>
    T* LoadObject(const FName& QualifiedName)
    {
        return (T*)StaticLoadObject(T::StaticClass(), QualifiedName);
    }

    template<typename T>
    T* LoadObject(const FName& Package, const FName& Name)
    {
        FString QualifiedName = Package.ToString() + "." + Name.ToString();
        return (T*)StaticLoadObject(T::StaticClass(), FName(QualifiedName));
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
    
    enum class EPropertyFlags : uint8
    {
        None = 0,
        PF_Const,
        PF_Private,
        PF_Protected,
        PF_SubField,
    };

    ENUM_CLASS_FLAGS(EPropertyFlags);

    /** This must reflect EPropertyTypeFlags found in ReflectedType.h */
    enum class EPropertyTypeFlags : uint8
    {
        None = 0,

        // Signed integers
        Int8,
        Int16,
        Int32,
        Int64,

        // Unsigned integers
        UInt8,
        UInt16,
        UInt32,
        UInt64,

        // Floats
        Float,
        Double,

        // Other types
        Bool,
        Object,
        Class,
        Name,
        String,
        Enum,
        Vector,
        Struct,

        Count,
    };

    ENUM_CLASS_FLAGS(EPropertyTypeFlags);
    
    inline constexpr const char* PropertyTypeFlagNames[] = {
        "None",
        "Int8",
        "Int16",
        "Int32",
        "Int64",

        "UInt8",
        "UInt16",
        "UInt32",
        "UInt64",

        "Float",
        "Double",

        "Bool",
        "Object",
        "Class",
        "Name",
        "String",
        "Enum",
        "Vector",
        "Struct"
    };

    static_assert(std::size(PropertyTypeFlagNames) == (SIZE_T)EPropertyTypeFlags::Count, "PropertyTypeFlagStrings must match number of flags in EPropertyTypeFlags");
    
    inline const char* PropertyTypeToString(EPropertyTypeFlags Flag)
    {
        size_t index = static_cast<size_t>(Flag);
        if (index >= std::size(PropertyTypeFlagNames))
            return "Invalid";
        return PropertyTypeFlagNames[index];
    }
    
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

    typedef void (*SetterFuncPtr)(void* InContainer, const void* InValue);
    typedef void (*GetterFuncPtr)(const void* InContainer, void* OutValue);

    // Add an element to the array
    typedef void (*ArrayPushBackPtr)(void* InContainer, const void* InValue);

    // Get the number of elements in the array
    typedef size_t (*ArrayGetNumPtr)(const void* InContainer);

    // Remove an element at a given index
    typedef void (*ArrayRemoveAtPtr)(void* InContainer, size_t Index);

    // Clear the array
    typedef void (*ArrayClearPtr)(void* InContainer);
    
    // Access an element by index (mutable)
    typedef void* (*ArrayGetAtPtr)(void* InContainer, size_t Index);

    
    struct FPropertyParams
    {
        const char*         Name;
        EPropertyFlags      PropertyFlags;
        EPropertyTypeFlags  TypeFlags;
        SetterFuncPtr       SetterFunc;
        GetterFuncPtr       GetterFunc;
        uint16              Offset;
    };

    struct FNumericPropertyParams : FPropertyParams
    {
        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FStringPropertyParams : FPropertyParams
    {
        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FNamePropertyParams : FPropertyParams
    {
        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FObjectPropertyParams : FPropertyParams
    {
        CClass*            (*ClassFunc)();

        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FClassPropertyParams : FPropertyParams
    {
        CClass*            (*ClassFunc)();

        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FStructPropertyParams : FPropertyParams
    {
        CStruct*            (*StructFunc)();

        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FEnumPropertyParams : FPropertyParams
    {
        CEnum*              (*EnumFunc)();

        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
    };

    struct FArrayPropertyParams : FPropertyParams
    {
        ArrayPushBackPtr    PushBackFn;
        ArrayGetNumPtr      GetNumFn;
        ArrayRemoveAtPtr    RemoveAtFn;
        ArrayClearPtr       ClearFn;
        ArrayGetAtPtr       GetAtFn;
        
        #ifdef WITH_DEVELOPMENT_TOOLS
        uint16 NumMetaData;
        const FMetaDataPairParam* MetaDataArray;
        #endif
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
