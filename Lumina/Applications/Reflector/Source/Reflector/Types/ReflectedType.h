#pragma once
#include <sstream>

#include "EASTL/string.h"
#include "EASTL/vector.h"

namespace Lumina
{
    class FReflectedProperty;
}

/** This must reflect EPropertyTypeFlags found in ObjectCore.h */
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
};

namespace Lumina::Reflection
{
    inline EPropertyTypeFlags GetCoreTypeFromName(const char* Name)
    {
        if (strcmp(Name, "bool") == 0)      return EPropertyTypeFlags::Bool;
        if (strcmp(Name, "uint8") == 0)     return EPropertyTypeFlags::UInt8;
        if (strcmp(Name, "uint16") == 0)    return EPropertyTypeFlags::UInt16;
        if (strcmp(Name, "uint32") == 0)    return EPropertyTypeFlags::UInt32;
        if (strcmp(Name, "uint64") == 0)    return EPropertyTypeFlags::UInt64;
        if (strcmp(Name, "int8") == 0)      return EPropertyTypeFlags::Int8;
        if (strcmp(Name, "int16") == 0)     return EPropertyTypeFlags::Int16;
        if (strcmp(Name, "int32") == 0)     return EPropertyTypeFlags::Int32;
        if (strcmp(Name, "int64") == 0)     return EPropertyTypeFlags::Int64;
        if (strcmp(Name, "float") == 0)     return EPropertyTypeFlags::Float;
        if (strcmp(Name, "double") == 0)    return EPropertyTypeFlags::Double;
        if (strcmp(Name, "CObject") == 0)   return EPropertyTypeFlags::Object;
        if (strcmp(Name, "CClass") == 0)    return EPropertyTypeFlags::Class;
        if (strcmp(Name, "FName") == 0)     return EPropertyTypeFlags::Name;
        if (strcmp(Name, "FString") == 0)   return EPropertyTypeFlags::String;

        return EPropertyTypeFlags::None;
    }
    
    /** Abstract base to all reflected types */
    class FReflectedType
    {
    public:

        enum class EType : uint8_t
        {
            Class,
            Structure,
            Enum,
        };
        
        virtual ~FReflectedType() = default;
        virtual void DefineConstructionStatics(std::stringstream& SS) = 0;

        uint32_t        GeneratedBodyLineNumber;
        uint32_t        LineNumber;
        eastl::string   ID;
        eastl::string   HeaderID;
        eastl::string   DisplayName;
        eastl::string   QualifiedName;
        eastl::string   Namespace;
        EType           Type;
        
    };
    

    /** Reflected enumeration. */
    class FReflectedEnum : public FReflectedType
    {
    public:

        struct FConstant
        {
            eastl::string ID;
            eastl::string Label;
            eastl::string Description;
            uint32_t Value;
        };

        FReflectedEnum()
        {
            Type = EType::Enum;
        }
        
        void DefineConstructionStatics(std::stringstream& SS) override;

        void AddConstant(const FConstant& Constant) { Constants.push_back(Constant); }

        eastl::vector<FConstant> Constants;
        
        
    };

    
    /** Reflected structure */
    class FReflectedStruct : public FReflectedType
    {
    public:

        virtual ~FReflectedStruct();
        
        FReflectedStruct()
        {
            Type = EType::Structure;
        }

        template<typename T>
        T* PushProperty()
        {
            T* New = new T;
            Props.push_back(New);
            return New;
        }

        void DefineConstructionStatics(std::stringstream& SS) override;
        
        eastl::vector<const FReflectedProperty*>      Props;

        eastl::string                                 Parent;
    };

    
    /** Reflected class */
    class FReflectedClass : public FReflectedStruct
    {
    public:
        FReflectedClass()
        {
            Type = EType::Class;
        }
        
        void DefineConstructionStatics(std::stringstream& SS) override;

    };
}
