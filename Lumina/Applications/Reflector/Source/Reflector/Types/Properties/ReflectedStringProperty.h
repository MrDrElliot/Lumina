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
            if (bInner)
            {
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::String," +  " 0, " " };\n";
            }
            else
            {
                if (Metadata.empty())
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::String,," +  " offsetof(" + Outer + ", " + Name + ") " " };\n";
                }
                else
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::String,," +  " offsetof(" + Outer + ", " + Name + "), " + " METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata) " + "};\n";
                }
            }
        }

        virtual const char* GetPropertyParamType() const override { return "FStringPropertyParams"; } \

    };

    class FReflectedNameProperty : public FReflectedProperty
    {
    public:

        const char* GetTypeName() override { return "FName"; }

        void AppendDefinition(eastl::string& Stream) const override
        {
            if (bInner)
            {
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Name," +  " 0, " " };\n";
            }
            else
            {
                if (Metadata.empty())
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Name," +  " offsetof(" + Outer + ", " + Name + ") " " };\n";
                }
                else
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Name," +  " offsetof(" + Outer + ", " + Name + "), " + " METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata) " + "};\n";
                }
            }
        }

        virtual const char* GetPropertyParamType() const override { return "FNamePropertyParams"; } \

    };
}
