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
            if (bInner)
            {
            Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Enum," +  "0, Construct_CEnum_" + ClangUtils::StripNamespace(TypeName) + " };\n";
            }
            else
            {
                if (Metadata.empty())
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Enum," +  " offsetof(" + Outer + ", " + Name + "), Construct_CEnum_" + ClangUtils::StripNamespace(TypeName) + " };\n";
                }
                else
                {
                    Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Enum," +  " offsetof(" + Outer + ", " + Name + "), Construct_CEnum_" + ClangUtils::StripNamespace(TypeName) + ", METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata)" + "};\n";
                }
            }
            
        }

        const char* GetPropertyParamType() const override { return "FEnumPropertyParams"; }

    };
    
}
