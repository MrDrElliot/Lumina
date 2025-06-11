#pragma once
#include "ReflectedProperty.h"

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
            //@ TODO Not sure where to get the namespace the internal parameter comes from.
            //Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Object," +  " offsetof(" + Outer + ", " + Name + "), Construct_CClass_" + TypeName + " };\n";
            if (bInner)
            {
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Object," +  " 0, nullptr" " };\n";
            }
            else
            {
                Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Object," +  " offsetof(" + Outer + ", " + Name + "), nullptr" " };\n";
            }

        }

        const char* GetPropertyParamType() const override { return "FObjectPropertyParams"; }
    
    };
}
