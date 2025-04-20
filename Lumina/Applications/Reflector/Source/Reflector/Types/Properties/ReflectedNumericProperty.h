#pragma once
#include "ReflectedProperty.h"


namespace Lumina
{
#define DEFINE_REFLECTED_NUMERIC_PROPERTY(ClassName, TypeFlag, TypeNameStr) \
    class ClassName final : public FReflectedProperty \
    { \
    public: \
        virtual ~ClassName() = default; \
        void AppendDefinition(std::stringstream& SS) const override \
        { \
            AppendPropertyDef(SS, "EPropertyFlags::None", #TypeFlag); \
        } \
        const char* GetTypeName() override { return TypeNameStr; } \
    };

    // Unsigned integer properties
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedUInt8Property,  EPropertyTypeFlags::UInt8,  "UInt8")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedUInt16Property, EPropertyTypeFlags::UInt16, "UInt16")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedUInt32Property, EPropertyTypeFlags::UInt32, "UInt32")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedUInt64Property, EPropertyTypeFlags::UInt64, "UInt64")

    // Signed integer properties
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedInt8Property,   EPropertyTypeFlags::Int8,  "Int8")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedInt16Property,  EPropertyTypeFlags::Int16, "Int16")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedInt32Property,  EPropertyTypeFlags::Int32, "Int32")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedInt64Property,  EPropertyTypeFlags::Int64, "Int64")

    // Floating point properties
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedFloatProperty,  EPropertyTypeFlags::Float,  "Float")
    DEFINE_REFLECTED_NUMERIC_PROPERTY(FReflectedDoubleProperty, EPropertyTypeFlags::Double, "Double")

    #undef DEFINE_REFLECTED_NUMERIC_PROPERTY
    
}
