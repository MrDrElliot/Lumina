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
            Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::String," +  " offsetof(" + Outer + ", " + Name + ") };\n";
        }
    
    };

    class FReflectedNameProperty : public FReflectedProperty
    {
    public:

        const char* GetTypeName() override { return "FName"; }

        void AppendDefinition(eastl::string& Stream) const override
        {
            Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Name," +  " offsetof(" + Outer + ", " + Name + ") };\n";
        }
    };
}
