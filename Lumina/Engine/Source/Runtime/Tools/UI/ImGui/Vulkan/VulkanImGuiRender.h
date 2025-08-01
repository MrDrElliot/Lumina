#pragma once

#include <vulkan/vulkan_core.h>

#include "GUID/GUID.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    class FVulkanRenderContext;
    class FUpdateContext;

    class FVulkanImGuiRender: public IImGuiRenderer
    {
    public:
        
        void Initialize(FSubsystemManager& Manager) override;
        void Deinitialize() override;
        
        void OnStartFrame(const FUpdateContext& UpdateContext) override;
        void OnEndFrame(const FUpdateContext& UpdateContext) override;

        /** An ImTextureID in this context is castable to a VkDescriptorset. */
        ImTextureID GetOrCreateImTexture(FRHIImageRef Image) override;

    private:
        
        VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
        FVulkanRenderContext* VulkanRenderContext = nullptr;

        THashMap<VkImage, VkDescriptorSet> ImageCache;
    };
    
}
