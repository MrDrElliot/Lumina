#include "RenderComponent.h"

namespace Lumina
{
    void SRenderComponent::MarkRenderStateDirty()
    {
        EntityPrivate.GetOrAddComponent<FDirtyRenderStateComponent>().bNeedsRenderProxyUpdate = true;
    }

    void SRenderComponent::MarkRenderTransformDirty()
    {
        EntityPrivate.GetOrAddComponent<FDirtyRenderStateComponent>().bNeedsTransformUpdate = true;
    }
}
