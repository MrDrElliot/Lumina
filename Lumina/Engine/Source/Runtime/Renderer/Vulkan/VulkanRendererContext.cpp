#include "VulkanRendererContext.h"

#include "GLFW/glfw3.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    
    FVulkanRendererContext::FVulkanRendererContext()
    {
        LogicalDevice = nullptr;
        PhysicalDevice = nullptr;
    }

    FVulkanRendererContext::~FVulkanRendererContext()
    {
        vkDestroyInstance(Instance, nullptr);
        Instance = nullptr;
    }

    void FVulkanRendererContext::Init()
    {

        if(!glfwVulkanSupported()) LE_LOG_CRITICAL("Vulkan Not Supported!");

        
    }
}
