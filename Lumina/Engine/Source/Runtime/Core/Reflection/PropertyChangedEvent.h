#pragma once
#include "Containers/Name.h"


namespace Lumina
{
    class CStruct;
    class FProperty;
}

namespace Lumina
{

    struct FPropertyChangedEvent
    {

        /** Struct in which this property is owned by */
        CStruct*    OuterStruct;

        /** Property in which was changed. */
        FProperty*  Property;

        /** Name of the property changed */
        FName       PropertyName;
        
    };

    
}
