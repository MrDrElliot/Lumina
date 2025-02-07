#pragma once
#include <vulkan/vulkan_core.h>

#include "Renderer/Semaphore.h"


namespace Lumina
{
    class FVulkanSemaphore : public FSemaphore
    {
    public:

        FVulkanSemaphore();
        ~FVulkanSemaphore();

        VkSemaphore GetVkSemaphore() const { return Semaphore; }
        void SetFriendlyName(const FString& InName) override;

    private:

        VkSemaphore Semaphore;
        
    };
    
}
