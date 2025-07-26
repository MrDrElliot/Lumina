#pragma once
#include "ReflectedProperty.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina
{
    class FReflectedStructProperty : public FReflectedProperty
    {
    public:

        const char* GetTypeName() override
        {
            return "Struct";
        }
        
        void AppendDefinition(eastl::string& Stream) const override
        {
            if (bInner)
            {
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Struct," +  " 0, Construct_CStruct_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + " };\n";
            }
            else
            {
                if (Metadata.empty())
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Struct," +  " offsetof(" + Outer + ", " + Name + "), Construct_CStruct_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + " };\n";
                }
                else
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Struct," +  " offsetof(" + Outer + ", " + Name + "), Construct_CStruct_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + ", METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata) " + "};\n";
                }
            }

        }

        const char* GetPropertyParamType() const override { return "FStructPropertyParams"; }

        void DeclareCrossModuleReference(const eastl::string& API, eastl::string& Stream) override
        {
            Stream += API;
            Stream += " Lumina::CStruct* Construct_CStruct_";
            Stream += ClangUtils::MakeCodeFriendlyNamespace(TypeName);
            Stream += "();\n";
        }
    };
}
