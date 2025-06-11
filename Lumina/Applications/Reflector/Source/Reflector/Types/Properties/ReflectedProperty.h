#pragma once
#include <sstream>

#include <EASTL/string.h>

namespace Lumina
{
    class FReflectedProperty
    {
    public:

        virtual void AppendDefinition(eastl::string& Stream) const = 0;
        void AppendPropertyDef(eastl::string& Stream, const char* PropertyFlags, const char* TypeFlags) const;
        
        virtual const char* GetPropertyParamType() const { return "FPropertyParams"; }

        virtual const char* GetTypeName() = 0;
        eastl::string GetDisplayName() const { return Name; }

        eastl::string   TypeName;
        eastl::string   Namespace;
        eastl::string   Name;
        eastl::string   Outer;
        bool            bInner = false;
    };
}
