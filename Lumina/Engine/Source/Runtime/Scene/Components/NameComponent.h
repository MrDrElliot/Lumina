#pragma once
#include "Containers/String.h"

namespace Lumina
{
    struct NameComponent
    {
        NameComponent() = default;
        NameComponent(const LString& InName) :Name(InName) {}
        NameComponent(const char* InName) :Name(InName) {}

        LString& GetName() { return Name; }

    private:
    
        LString Name;
    };
}
