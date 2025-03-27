#pragma once
#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    enum class EFieldType : uint32
    {
        Float,
        Integer,
        Enum,
        Boolean,
        String,
    };

    enum class EFieldFlags : uint32
    {
        None      = 0,
        Editable  = 1 << 0,
        Nullable  = 1 << 1,  // Example: allow null values for the field
        Default   = 1 << 2,  // Example: a default value is provided for the field
    };

    ENUM_CLASS_FLAGS(EFieldFlags)

    // Struct to hold information about a single field in a class
    struct FField
    {
        EFieldType  Type;          // Type of the field (e.g., Float, Integer)
        FString     FieldName;     // Name of the field
        uint32      Offset;        // Offset of the field in the class (for serialization or reflection)
        uint32      Size;          // Size of the field
        EFieldFlags Flags;         // Flags describing field properties (e.g., editable, nullable)
        uint32      DefaultValue;  // Default value (if any)
    };
    

    // Template to handle field sizes automatically
    template<EFieldType T>
    struct DatamapFieldSize
    {
        enum { SIZE = 0 };
        static int GetSize() { return 0; }
    };

    // Macro for defining field sizes for specific types
    #define DECLARE_FIELD_SIZE(FieldType, FieldSize) \
    template<> struct DatamapFieldSize<FieldType> { enum { SIZE = FieldSize }; static int GetSize() { return FieldSize; }}; \

    // Define the field sizes for various types
    DECLARE_FIELD_SIZE(EFieldType::Float, sizeof(float));
    DECLARE_FIELD_SIZE(EFieldType::Integer, sizeof(int));
    DECLARE_FIELD_SIZE(EFieldType::Boolean, sizeof(bool));
    DECLARE_FIELD_SIZE(EFieldType::String, sizeof(void*));

    #undef DECLARE_FIELD_SIZE

    // Macro for defining class fields
    #define CLASS_FIELD(Name, FieldType, Count, Flags) \
        { FieldType, #Name, offsetof(ThisClass, Name), DatamapFieldSize<FieldType>::SIZE * (Count), Flags }

    // Define a helper macro for class fields with only one item
    #define DEFINE_FIELD(Name, FieldType, Flags) CLASS_FIELD(Name, EFieldType::FieldType, 1, Flags)
    
    
}
