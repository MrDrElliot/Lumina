#include "RenderManager.h"

#if LUMINA_RENDERER_VULKAN
#include "API/Vulkan/VulkanRenderContext.h"
#include "Tools/UI/ImGui/Vulkan/VulkanImGuiRender.h"
#endif

#include "Core/Profiler/Profile.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    void FRenderManager::Initialize(FSubsystemManager& Manager)
    {
        RenderContext = Memory::New<FVulkanRenderContext>();
        RenderContext->Initialize();

        FSamplerDesc SamplerDesc; SamplerDesc
        .SetAllFilters(false)
        .SetAllAddressModes(ESamplerAddressMode::Repeat);
        NearestSamplerRepeat = RenderContext->CreateSampler(SamplerDesc);

        SamplerDesc.SetAllFilters(false)
        .SetAllAddressModes(ESamplerAddressMode::Clamp);
        NearestSamplerClamped = RenderContext->CreateSampler(SamplerDesc);
        
        SamplerDesc.SetAllFilters(true)
        .SetAllAddressModes(ESamplerAddressMode::Clamp);
        LinearSampler = RenderContext->CreateSampler(SamplerDesc);

        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer = Memory::New<FVulkanImGuiRender>();
        ImGuiRenderer->Initialize(Manager);
        #endif
    }

    void FRenderManager::Deinitialize()
    {
        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer->Deinitialize();
        Memory::Delete(ImGuiRenderer);
        #endif

        NearestSamplerRepeat.SafeRelease();
        NearestSamplerClamped.SafeRelease();
        LinearSampler.SafeRelease();
        
        RenderContext->Deinitialize();
        Memory::Delete(RenderContext);
    }

    void FRenderManager::FrameStart(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        RenderContext->FrameStart(UpdateContext, CurrentFrameIndex);

        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer->StartFrame(UpdateContext);
        #endif
    }

    void FRenderManager::FrameEnd(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer->EndFrame(UpdateContext);
        #endif
        
        RenderContext->FrameEnd(UpdateContext);

        RenderContext->FlushPendingDeletes();
        
        CurrentFrameIndex = (CurrentFrameIndex + 1) % FRAMES_IN_FLIGHT;
    }
}
