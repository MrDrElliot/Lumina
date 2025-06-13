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
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Struct," +  " offsetof(" + Outer + ", " + Name + "), Construct_CStruct_" + ClangUtils::MakeCodeFriendlyNamespace(TypeName) + " };\n";
            }

        }

        const char* GetPropertyParamType() const override { return "FStructPropertyParams"; }
        
    };
}
