#pragma once
#include "Containers/Name.h"
#include "Core/Object/ObjectCore.h"
#include "Memory/Memory.h"
#include <sstream>


namespace Lumina
{
    class FReflectedProperty;
}

namespace Lumina
{
    enum class EPropertyTypeFlags : uint16;
}

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

        enum class EType : uint8
        {
            Class,
            Structure,
            Enum,
        };
        
        virtual ~FReflectedType() = default;
        virtual void DefineConstructionStatics(std::stringstream& SS) = 0;

        uint32      GeneratedBodyLineNumber;
        uint32      LineNumber;
        FName       ID;
        FName       HeaderID;
        FString     DisplayName;
        EType       Type;
        
    };
    

    /** Reflected enumeration. */
    class FReflectedEnum : public FReflectedType
    {
    public:

        struct FConstant
        {
            FName ID;
            FString Label;
            FString Description;
            uint32 Value;
        };

        FReflectedEnum()
        {
            Type = EType::Enum;
        }
        
        void DefineConstructionStatics(std::stringstream& SS) override;

        void AddConstant(const FConstant& Constant) { Constants.push_back(Constant); }

        TVector<FConstant> Constants;
        
        
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
            T* New = FMemory::New<T>();
            Props.push_back(New);
            return New;
        }

        void DefineConstructionStatics(std::stringstream& SS) override;
        
        TVector<const FReflectedProperty*>      Props;

        FString                                 Parent;
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
