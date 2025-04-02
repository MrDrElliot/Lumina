#pragma once
#include "Core/LuminaMacros.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    LUM_ENUM()
    enum class EObjectFlags : uint32
    {
        None            = 0,        /**< No special properties. */

        Abstract        = 1 << 0,    /**< Cannot be instantiated directly. */

        Transient       = 1 << 1,    /**< Object is not saved during serialization. */
        Persistent      = 1 << 2,    /**< Object should be saved and loaded. */

    };

    ENUM_CLASS_FLAGS(EObjectFlags);
}
