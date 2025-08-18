#include "ReflectedProperty.h"

#include <iostream>

#include "Reflector/Utils/MetadataUtils.h"

namespace Lumina
{
    void FReflectedProperty::AppendPropertyDef(eastl::string& Stream, const char* PropertyFlags, const char* TypeFlags, const eastl::string& CustomData) const
    {
        eastl::string GetterFunctionName = GetterFunc.empty() ? "nullptr" : Outer + "::" + GetterFunc + "_WrapperImpl";
        eastl::string SetterFunctionName = SetterFunc.empty() ? "nullptr" : Outer + + "::" + SetterFunc + "_WrapperImpl";

        Stream += "{ \"" + Name + "\", " + PropertyFlags + ", " + TypeFlags + ", " + SetterFunctionName + ", " + GetterFunctionName + ", ";

        if (bInner)
        {
            Stream += "0";
        }
        else
        {
            Stream += "offsetof(" + Outer + ", " + Name + ")";
        }

        if (!CustomData.empty())
        {
            Stream += ", " + CustomData;
        }

        if (!Metadata.empty())
        {
            Stream += ", METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata)";
        }

        Stream += " };\n";
    }

    void FReflectedProperty::GenerateMetadata(const eastl::string& InMetadata)
    {
        if (InMetadata.empty())
            return;
        
        FMetadataParser Parser(InMetadata);
        Metadata = eastl::move(Parser.Metadata);

        for (const FMetadataPair& MetadataPair : Metadata)
        {
            if (MetadataPair.Key == "Getter")
            {
                if (MetadataPair.Value.empty())
                {
                    GetterFunc = "Get" + Name;
                }
                else
                {
                    GetterFunc = MetadataPair.Value;
                }
            }

            if (MetadataPair.Key == "Setter")
            {
                if (MetadataPair.Value.empty())
                {
                    SetterFunc = "Set" + Name;
                }
                else
                {
                    SetterFunc = MetadataPair.Value;
                }
            }
        }
    }
}
