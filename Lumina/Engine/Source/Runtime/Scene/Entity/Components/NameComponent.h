#pragma once

#include "Component.h"
#include "Containers/String.h"

namespace Lumina
{
    class FNameComponent : public FEntityComponent
    {
    public:

        FNameComponent() = default;
        FNameComponent(const FString& InName) :Name(InName) {}
        FNameComponent(const char* InName) :Name(InName) {}
        
        FString& GetName() { return Name; }

    private:
    
        FString Name;
    };
}
