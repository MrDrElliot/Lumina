#include "VulkanSwapChain.h"

Lumina::FVulkanSwapChain::FVulkanSwapChain(VkInstance InInstance, LVulkanDevice* InDevice, GLFWwindow* InWindow)
{
    Instance = InInstance;
    Device = InDevice;

    VkDevice VulkanDevice = Device->GetVulkanDevice();
}

void Lumina::FVulkanSwapChain::Create(uint32_t* InWidth, uint32_t* InHeight, bool VSync)
{
}

void Lumina::FVulkanSwapChain::Destroy()
{
}
