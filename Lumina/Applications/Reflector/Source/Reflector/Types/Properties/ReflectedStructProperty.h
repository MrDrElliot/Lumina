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
            eastl::string CustomData = "Construct_CStruct_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName);
            AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::Struct", CustomData);
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
