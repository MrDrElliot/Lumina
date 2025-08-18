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
            eastl::string CustomData = "Construct_CEnum_" + ClangUtils::StripNamespace(TypeName);
            AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::Enum", CustomData);
        }

        const char* GetPropertyParamType() const override { return "FEnumPropertyParams"; }

    };
    
}
