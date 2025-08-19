#pragma once

#include "Component.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "World/Entity/Entity.h"
#include "RenderComponent.generated.h"

namespace Lumina
{
    class CWorld;
}

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SRenderComponent : SEntityComponent
    {
        GENERATED_BODY()
        friend class CWorld;
        
    protected:
        
        virtual void MarkRenderStateDirty();
        virtual void MarkRenderTransformDirty();

    private:

        /** Render components must know their owning
         * entity due to needing to update their render state
         * by adding a RenderStateDirty component, access to this
         * is kept private for safety. */
        Entity EntityPrivate = {};
    };


    struct FDirtyRenderStateComponent : SEntityComponent
    {
        uint8 bNeedsTransformUpdate:1 = 0;
        uint8 bNeedsRenderProxyUpdate:1 = 0;
    };
}
