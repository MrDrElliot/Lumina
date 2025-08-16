#pragma once


namespace Lumina
{

    struct SEditorComponent : SEntityComponent
    {
        bool bEnabled = true;
    };

    struct SHiddenComponent : SEntityComponent { };
}
