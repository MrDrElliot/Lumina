#include "SceneUpdateContext.h"

#include "Scene.h"

namespace Lumina
{
    FSceneUpdateContext::FSceneUpdateContext(const FUpdateContext& ParentContext, FScene* InScene)
        :FUpdateContext(ParentContext)
        , Scene(InScene)
    {
        Assert(Scene != nullptr);
    }

    FSubsystemManager* FSceneUpdateContext::GetSceneSubsystemManager() const
    {
        return Scene->GetSceneSubsystemManager();
    }
}
