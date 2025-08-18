#pragma once
#include "ReflectedProperty.h"

namespace Lumina
{
    class FReflectedStringProperty : public FReflectedProperty
    {
    public:

        const char* GetTypeName() override { return "FString"; }

        void AppendDefinition(eastl::string& Stream) const override
        {
            AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::String");
        }

        virtual const char* GetPropertyParamType() const override { return "FStringPropertyParams"; } \

    };

    class FReflectedNameProperty : public FReflectedProperty
    {
    public:

        const char* GetTypeName() override { return "FName"; }

        void AppendDefinition(eastl::string& Stream) const override
        {
            AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::Name");
        }

        virtual const char* GetPropertyParamType() const override { return "FNamePropertyParams"; } \

    };
}
