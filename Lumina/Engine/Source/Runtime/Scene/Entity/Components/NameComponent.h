#pragma once

#include "Component.h"
#include "Containers/Name.h"

namespace Lumina
{
    struct FNameComponent : FEntityComponent
    {
        FNameComponent(const FName& InName) :Name(InName) {}
        FName Name;
    };
    
}
