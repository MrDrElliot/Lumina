#include "RenderManager.h"

#if LUMINA_RENDERER_VULKAN
#include "API/Vulkan/VulkanRenderContext.h"
#include "Tools/UI/ImGui/Vulkan/VulkanImGuiRender.h"
#endif

#include "RHIGlobals.h"
#include "Core/Profiler/Profile.h"
#include "Tools/UI/UITextureCache.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    void FRenderManager::Initialize(FSubsystemManager& Manager)
    {
        GRenderContext = Memory::New<FVulkanRenderContext>();
        GRenderContext->Initialize();

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
        FUITextureCache::Get().Clear();
        #endif

        
        GRenderContext->Deinitialize();
        Memory::Delete(GRenderContext);
        GRenderContext = nullptr;
    }

    void FRenderManager::FrameStart(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        GRenderContext->FrameStart(UpdateContext, CurrentFrameIndex);

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
        
        GRenderContext->FrameEnd(UpdateContext);

        GRenderContext->FlushPendingDeletes();
        
        CurrentFrameIndex = (CurrentFrameIndex + 1) % FRAMES_IN_FLIGHT;
    }
}
