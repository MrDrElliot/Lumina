#pragma once
#include "ReflectedProperty.h"


namespace Lumina
{
    class FReflectedEnumProperty : public FReflectedProperty
    {
    public:
        
        const char* GetTypeName() override
        {
            return "Enum";
        }
        
        void AppendDefinition(eastl::string& Stream) const override
        {
            Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Enum," +  " offsetof(" + Outer + ", " + Name + "), Construct_CEnum_" + ClangUtils::StripNamespace(TypeName) + " };\n";
        }

        const char* GetPropertyParamType() const override { return "FEnumPropertyParams"; }

    };
    
}
