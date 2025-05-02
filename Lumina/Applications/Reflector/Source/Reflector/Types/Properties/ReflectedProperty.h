#pragma once
#include "Containers/String.h"
#include <sstream>

namespace Lumina
{
    class FReflectedProperty
    {
    public:

        virtual void AppendDefinition(std::stringstream& SS) const = 0;
        void AppendPropertyDef(std::stringstream& SS, const char* PropertyFlags, const char* TypeFlags) const;


        virtual const char* GetTypeName() = 0;
        FString GetDisplayName() const { return Name; }
        
        FString Name;
        FString Outer;
    };
}
