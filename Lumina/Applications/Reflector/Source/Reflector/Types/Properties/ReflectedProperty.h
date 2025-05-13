#pragma once
#include <sstream>

#include "EASTL/string.h"

namespace Lumina
{
    class FReflectedProperty
    {
    public:

        virtual void AppendDefinition(std::stringstream& SS) const = 0;
        void AppendPropertyDef(std::stringstream& SS, const char* PropertyFlags, const char* TypeFlags) const;


        virtual const char* GetTypeName() = 0;
        eastl::string GetDisplayName() const { return Name; }
        
        eastl::string Name;
        eastl::string Outer;
    };
}
