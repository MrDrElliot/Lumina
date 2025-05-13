#pragma once
#include "Core/LuminaMacros.h"


namespace Lumina
{
    enum EObjectFlags
    {
        OF_None                 = 0,
        
        OF_Transient            = 1 << 0,
        OF_Persistent           = 1 << 1,

        OF_DefaultObject        = 1 << 2,

        OF_PendingDelete        = 1 << 3,

        OF_CompiledInNative     = 1 << 4,

    };

    ENUM_CLASS_FLAGS(EObjectFlags);
}
