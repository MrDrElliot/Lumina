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
        
        void AppendDefinition(std::stringstream& SS) const override
        {
            SS << "{ " << "\"" <<  Name.c_str() << "\"" << ", Lumina::EPropertyFlags::None, " << "Lumina::EPropertyTypeFlags::Enum," <<  " offsetof(" << Outer.c_str() << ", " << Name.c_str() << "), Construct_CEnum_" << TypeName.c_str()  << " };\n";

        }

        const char* GetPropertyParamType() const override { return "FEnumPropertyParams"; }

    };
    
}
