#pragma once
#include "Core/LuminaMacros.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    enum class EObjectFlags : uint32
    {
        None            = 0,

        Abstract        = 1 << 0,

        Transient       = 1 << 1,
        Persistent      = 1 << 2,

    };

    ENUM_CLASS_FLAGS(EObjectFlags);
}
