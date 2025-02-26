#pragma once

#include <vulkan/vulkan_core.h>

#include "Scene/SceneRenderer.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    class FUpdateContext;

    class FVulkanImGuiRender: public IImGuiRenderer
    {
    public:

        FVulkanImGuiRender();
        ~FVulkanImGuiRender() override = default;

        void Initialize(FSubsystemManager& Manager) override;
        void Deinitialize() override;
        
        void OnStartFrame(FRenderManager* RenderManager) override;
        void OnEndFrame(FRenderManager* RenderManager) override;
    

    private:

        /** An ImTextureID in this context is castable to a VkDescriptorset. */
        void RegisterNewImage(FGuid Guid, ImTextureID NewImage, const FString& DebugName);
        
        VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
        
    };
    
}
