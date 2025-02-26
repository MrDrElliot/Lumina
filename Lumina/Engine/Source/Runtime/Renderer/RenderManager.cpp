#include "RenderManager.h"

#include "API/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
    void FRenderManager::Initialize(FSubsystemManager& Manager)
    {
        RenderContext = FMemory::New<FVulkanRenderContext>();
        RenderContext->Initialize();
    }

    void FRenderManager::Deinitialize()
    {
        RenderContext->Deinitialize();
        FMemory::Delete(RenderContext);
    }

    void FRenderManager::FrameStart(const FUpdateContext& UpdateContext)
    {
        CurrentFrameIndex = (CurrentFrameIndex + 1) % FRAMES_IN_FLIGHT;
        
        RenderContext->FrameStart(UpdateContext, CurrentFrameIndex);
    }

    void FRenderManager::FrameEnd(const FUpdateContext& UpdateContext)
    {
        RenderContext->FrameEnd(UpdateContext, CurrentFrameIndex);
    }
}
