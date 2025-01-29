#pragma once

#include "Component.h"
#include "Containers/String.h"

namespace Lumina
{
    class FNameComponent : public FComponent
    {
    public:

        FNameComponent() = default;
        FNameComponent(const FString& InName) :Name(InName) {}
        FNameComponent(const char* InName) :Name(InName) {}

        void Serialize(FArchive& Ar) override {}

        FString& GetName() { return Name; }

    private:
    
        FString Name;
    };
}
