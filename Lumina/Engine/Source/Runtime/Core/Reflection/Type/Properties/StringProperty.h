#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FStringProperty : public FProperty
    {
    public:

        FStringProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
        }
    
    };


    class FNameProperty : public FProperty
    {
    public:
        
        FNameProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
        }

    };
    
}
