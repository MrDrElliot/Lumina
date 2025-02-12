#pragma once
#include "Component.h"
#include "GUID/GUID.h"

namespace Lumina
{
    class FGUIDComponent : public FEntityComponent
    {
    public:

        FGUIDComponent(const FGuid& InGuid) :Guid(InGuid) {}

        inline FGuid GetGUID() const { return Guid; }

    private:

        FGuid Guid;   
    
    };
}
