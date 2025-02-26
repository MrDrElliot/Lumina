#pragma once

#include <vulkan/vulkan_core.h>

#include "Scene/SceneRenderer.h"
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
    

    private:

        /** An ImTextureID in this context is castable to a VkDescriptorset. */
        void RegisterNewImage(FGuid Guid, ImTextureID NewImage, const FString& DebugName);
        
        VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
        FVulkanRenderContext* VulkanRenderContext = nullptr;
    };
    
}
