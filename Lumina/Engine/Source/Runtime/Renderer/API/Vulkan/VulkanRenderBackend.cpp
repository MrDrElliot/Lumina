

#include "VulkanRenderDevice.h"
#include "Containers/Array.h"
#include "Memory/Memory.h"
#ifdef LUMINA_RENDERER_VULKAN
#include "Core/Assertions/Assert.h"
#include "GLFW/glfw3.h"
#include "Log/Log.h"
#include "src/VkBootstrap.h"
#include "Renderer/API/VulkanRenderBackend.h"
#include <vulkan/vulkan.hpp>

namespace Lumina
{
    VkBool32 VKAPI_PTR VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                LOG_INFO("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                LOG_DEBUG("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                LOG_WARN("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                LOG_ERROR("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                std::unreachable();
                break;
        }

        return VK_FALSE;
    }

    // We create one command pool per thread.
    struct FCommandPools
    {
        TVector<VkCommandPool> CommandPools;
    } CommandPools;
    
    struct FVulkanBackend
    {
        VkInstance                          VulkanInstance;
        VkPhysicalDevice                    PhysicalDevice;
        
        VkDevice                            Device;
        VkPhysicalDeviceProperties          DeviceProperties;
        VkPhysicalDeviceMemoryProperties    DeviceMemoryProperties;

        VkSwapchainKHR                      Swapchain;
        
        
        
    } Backend;
    
    struct FVulkanRenderContextFunctions
    {
        VkDebugUtilsMessengerEXT DebugMessenger;
        PFN_vkSetDebugUtilsObjectNameEXT DebugUtilsObjectNameEXT;
    } DebugUtils;
    
    
    void FVulkanRenderBackend::Initialize()
    {
        AssertMsg(glfwVulkanSupported(), "Vulkan Is Not Supported!");

        vkb::InstanceBuilder Builder;
        auto InstBuilder = Builder.set_app_name("Lumina Engine")
        .request_validation_layers()
        .use_default_debug_messenger()
        .set_debug_callback(VkDebugCallback)
        .enable_extension("VK_EXT_debug_utils")
        .require_api_version(1, 3, 0)
        .build();

        Backend.VulkanInstance = InstBuilder.value();
        
        DebugUtils.DebugMessenger = InstBuilder->debug_messenger;
        DebugUtils.DebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(vkGetInstanceProcAddr(
            Backend.VulkanInstance, "vkSetDebugUtilsObjectNameEXT"));
        
        
        FVulkanRenderDevice* VkRenderDevice = FMemory::New<FVulkanRenderDevice>();
        VkRenderDevice->CreateDevice(InstBuilder.value());
        
        RenderDevice = VkRenderDevice;
        RenderDevice->Initialize();

        for(uint32 i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            VkCommandPoolCreateFlags Flags;
            
            VkCommandPoolCreateInfo CreateInfo = {};
            CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            CreateInfo.flags = Flags;

            VkCommandPool Pool;
            vkCreateCommandPool(Backend.Device, &CreateInfo, nullptr, &Pool);
            CommandPools.CommandPools.push_back(Pool);
        }
    }
}



#endif