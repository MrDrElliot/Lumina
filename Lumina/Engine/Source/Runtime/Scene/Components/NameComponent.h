#pragma once

#include "Component.h"
#include "Containers/String.h"

namespace Lumina
{
    class FNameComponent : public FComponent
    {
    public:

        FNameComponent() = default;
        FNameComponent(const LString& InName) :Name(InName) {}
        FNameComponent(const char* InName) :Name(InName) {}

        void Serialize(FArchive& Ar) override {}

        LString& GetName() { return Name; }

    private:
    
        LString Name;
    };
}
