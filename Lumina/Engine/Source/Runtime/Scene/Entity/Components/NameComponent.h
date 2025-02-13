#pragma once

#include "Component.h"
#include "Containers/Name.h"

namespace Lumina
{
    class FNameComponent : public FEntityComponent
    {
    public:

        FNameComponent() = default;
        FNameComponent(const FName& InName) :Name(InName) {}
        FNameComponent(const char* InName) :Name(InName) {}
        
        const FName& GetName() const { return Name; }

    private:
    
        FName Name;
    };
}
