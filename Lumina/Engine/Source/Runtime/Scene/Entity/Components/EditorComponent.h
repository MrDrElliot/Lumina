#pragma once


namespace Lumina
{

    struct FEditorComponent : FEntityComponent
    {
        bool bEnabled = true;
    };

    struct FHiddenComponent : FEntityComponent { };
}
