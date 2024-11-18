#pragma once
#include "GUID/GUID.h"

namespace Lumina
{
    class FUniqueComponent
    {
    public:

        FUniqueComponent(const FGuid& InGuid) :Guid(InGuid) {}

        inline FGuid GetGUID() const { return Guid; }

    private:

        FGuid Guid;   
    
    };
}
