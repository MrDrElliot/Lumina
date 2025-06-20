#include "ReflectedProperty.h"

#include <iostream>

#include "Reflector/Utils/MetadataUtils.h"

namespace Lumina
{
    void FReflectedProperty::AppendPropertyDef(eastl::string& Stream, const char* PropertyFlags, const char* TypeFlags) const
    {
        if (bInner)
        {
            Stream += "{ \"" +  Name + "\"" + ", " + PropertyFlags + ", " + TypeFlags + ", 0, };\n";
        }
        else
        {
            Stream += "{ \"" +  Name + "\"" + ", " + PropertyFlags + ", " + TypeFlags + ", offsetof(" + Outer + ", " + Name + "), };\n";
        }
    }

    void FReflectedProperty::GenerateMetadata(const eastl::string& InMetadata)
    {
        FMetadataParser Parser(InMetadata);
        Metadata = eastl::move(Parser.Metadata);
    }
}
