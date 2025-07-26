#pragma once
#include "ReflectedProperty.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina
{
    class FReflectedObjectProperty : public FReflectedProperty
    {
    public:

        const char* GetTypeName() override
        {
            return "Object";
        }
        
        void AppendDefinition(eastl::string& Stream) const override
        {
            if (bInner)
            {
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Object," +  " 0, Construct_CClass_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + " };\n";
            }
            else
            {
                if (Metadata.empty())
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Object," +  " offsetof(" + Outer + ", " + Name + "), Construct_CClass_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + " };\n";
                }
                else
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Object," +  " offsetof(" + Outer + ", " + Name + "), " + "Construct_CClass_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + ", METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata) " + "};\n";
                }
            }
        }

        const char* GetPropertyParamType() const override { return "FObjectPropertyParams"; }

        void DeclareCrossModuleReference(const eastl::string& API, eastl::string& Stream) override
        {
            Stream += API;
            Stream += " Lumina::CClass* Construct_CClass_";
            Stream += ClangUtils::MakeCodeFriendlyNamespace(TypeName);
            Stream += "();\n";
        }
    };
}
