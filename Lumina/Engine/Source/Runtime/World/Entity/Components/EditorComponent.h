#pragma once

#include "Component.h"

namespace Lumina
{
    struct SEditorComponent : SEntityComponent
    {
        bool bEnabled = true;
    };

    struct SHiddenComponent : SEntityComponent { };
}
