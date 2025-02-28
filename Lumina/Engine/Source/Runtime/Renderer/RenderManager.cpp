#include "RenderManager.h"

#include "API/Vulkan/VulkanRenderContext.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/Vulkan/VulkanImGuiRender.h"

namespace Lumina
{
    void FRenderManager::Initialize(FSubsystemManager& Manager)
    {
        RenderContext = FMemory::New<FVulkanRenderContext>();
        RenderContext->Initialize();


        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer = FMemory::New<FVulkanImGuiRender>();
        ImGuiRenderer->Initialize(Manager);
        #endif
    }

    void FRenderManager::Deinitialize()
    {
        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer->Deinitialize();
        FMemory::Delete(ImGuiRenderer);
        #endif
        
        RenderContext->Deinitialize();
        FMemory::Delete(RenderContext);
    }

    void FRenderManager::FrameStart(const FUpdateContext& UpdateContext)
    {
        CurrentFrameIndex = (CurrentFrameIndex + 1) % FRAMES_IN_FLIGHT;

        RenderContext->FrameStart(UpdateContext, CurrentFrameIndex);

        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer->StartFrame(UpdateContext);
        #endif
    }

    void FRenderManager::FrameEnd(const FUpdateContext& UpdateContext)
    {
        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer->EndFrame(UpdateContext);
        #endif
        
        RenderContext->FrameEnd(UpdateContext);
    }
}
