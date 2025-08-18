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
            eastl::string CustomData = "Construct_CClass_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName);
            AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::Object", CustomData);
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
