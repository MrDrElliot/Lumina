#include "VulkanRendererContext.h"

#include "Source/Runtime/Log/Log.h"


namespace Lumina
{



    LVulkanRendererContext::LVulkanRendererContext()
    : Instance(nullptr)
    {
    }

    LVulkanRendererContext::~LVulkanRendererContext()
    {
       // vkDestroyInstance(Instance, nullptr);
        Instance = nullptr;
    }

    void LVulkanRendererContext::Init()
    {
        LE_LOG_INFO("Attempting Vulkan Renderer Context Initialization");
        
        if(glfwVulkanSupported() == GLFW_FALSE)
        {
            LE_LOG_CRITICAL("Vulkan must be supported!");
            return;
        }

        VkApplicationInfo AppInfo = {};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Lumina";
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "Lumina";
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pApplicationInfo = &AppInfo;

        uint32_t GlfwExtensionCount = 0;

        const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

        CreateInfo.enabledExtensionCount = GlfwExtensionCount;
        CreateInfo.ppEnabledExtensionNames = GlfwExtensions;

        CreateInfo.enabledLayerCount = 0;

        const VkResult Result = VK_SUCCESS; ;// = vkCreateInstance(&CreateInfo, nullptr, &Instance);
        if(Result != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Vulkan Instance Failed!");
            return;
        }

    }
}
