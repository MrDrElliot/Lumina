#pragma once
#include <clang-c/Index.h>

#include "StringHash.h"
#include "EASTL/hash_map.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "Reflector/Utils/MetadataUtils.h"

namespace Lumina::Reflection
{
    class FReflectedProject;
}

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
    Vector              = 1 << 17,
    Struct              = 1 << 18,
};

namespace Lumina::Reflection
{
    inline EPropertyTypeFlags GetCoreTypeFromName(const char* Name)
    {
        if (Name == nullptr)                            return EPropertyTypeFlags::None;
        if (strcmp(Name, "bool") == 0)                  return EPropertyTypeFlags::Bool;
        if (strcmp(Name, "uint8") == 0)                 return EPropertyTypeFlags::UInt8;
        if (strcmp(Name, "uint16") == 0)                return EPropertyTypeFlags::UInt16;
        if (strcmp(Name, "uint32") == 0)                return EPropertyTypeFlags::UInt32;
        if (strcmp(Name, "uint64") == 0)                return EPropertyTypeFlags::UInt64;
        if (strcmp(Name, "int8") == 0)                  return EPropertyTypeFlags::Int8;
        if (strcmp(Name, "int16") == 0)                 return EPropertyTypeFlags::Int16;
        if (strcmp(Name, "int32") == 0)                 return EPropertyTypeFlags::Int32;
        if (strcmp(Name, "int64") == 0)                 return EPropertyTypeFlags::Int64;
        if (strcmp(Name, "float") == 0)                 return EPropertyTypeFlags::Float;
        if (strcmp(Name, "double") == 0)                return EPropertyTypeFlags::Double;
        if (strcmp(Name, "Lumina::CClass") == 0)        return EPropertyTypeFlags::Class;
        if (strcmp(Name, "Lumina::FName") == 0)         return EPropertyTypeFlags::Name;
        if (strcmp(Name, "Lumina::FString") == 0)       return EPropertyTypeFlags::String;
        if (strcmp(Name, "Lumina::TVector") == 0)       return EPropertyTypeFlags::Vector;
        if (strcmp(Name, "Lumina::TObjectHandle") == 0) return EPropertyTypeFlags::Object;
        if (strcmp(Name, "Lumina::CObject") == 0)       return EPropertyTypeFlags::Object;

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

        virtual eastl::string GetTypeName() const = 0;
        virtual void DefineConstructionStatics(eastl::string& Stream) = 0;
        virtual void DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID) = 0;
        virtual void DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID) = 0;
        virtual void DeclareImplementation(eastl::string& Stream) = 0;
        virtual void DeclareStaticRegistration(eastl::string& Stream) = 0;

        bool DeclareAccessors(eastl::string& Stream, const eastl::string& FileID);
        
        void GenerateMetadata(const eastl::string& InMetadata);

        eastl::vector<eastl::shared_ptr<FReflectedProperty>>    Props;
        eastl::string                                           Project;
        uint32_t                                                GeneratedBodyLineNumber;
        uint32_t                                                LineNumber;
        eastl::string                                           HeaderID;
        eastl::string                                           DisplayName;
        eastl::string                                           QualifiedName;
        eastl::string                                           Namespace;
        EType                                                   Type;
        eastl::vector<FMetadataPair>                            Metadata;
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

        eastl::string GetTypeName() const override { return "CEnum"; }
        void DefineConstructionStatics(eastl::string& Stream) override;
        void DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID) override;
        void DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID) override;
        void DeclareImplementation(eastl::string& Stream) override;
        void DeclareStaticRegistration(eastl::string& Stream) override;

        void AddConstant(const FConstant& Constant) { Constants.push_back(Constant); }

        eastl::vector<FConstant>    Constants;
        
    };

    
    /** Reflected structure */
    class FReflectedStruct : public FReflectedType
    {
    public:

        virtual ~FReflectedStruct() override;
        
        FReflectedStruct()
        {
            Type = EType::Structure;
        }

        void PushProperty(const eastl::shared_ptr<FReflectedProperty>& NewProperty);

        eastl::string GetTypeName() const override { return "CStruct"; }
        
        void DefineConstructionStatics(eastl::string& Stream) override;
        void DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID) override;
        void DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID) override;
        void DeclareImplementation(eastl::string& Stream) override;
        void DeclareStaticRegistration(eastl::string& Stream) override;
        
        eastl::string                                               Parent;
    };

    
    /** Reflected class */
    class FReflectedClass : public FReflectedStruct
    {
    public:
        FReflectedClass()
        {
            Type = EType::Class;
        }

        eastl::string GetTypeName() const override { return "CClass"; }
        void DefineConstructionStatics(eastl::string& Stream) override;
        void DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID) override;
        void DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID) override;
        void DeclareImplementation(eastl::string& Stream) override;
        void DeclareStaticRegistration(eastl::string& Stream) override;

    };
    
}
