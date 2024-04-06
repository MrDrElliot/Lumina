#include "VulkanSwapChain.h"

#include "VkBootstrap.h"
#include "VulkanHelpers.h"
#include "VulkanRenderContext.h"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"


namespace Lumina
{
    FVulkanSwapChain::FVulkanSwapChain()
    {
    }

    FVulkanSwapChain::~FVulkanSwapChain()
    {
    }

    FVulkanSwapChain* FVulkanSwapChain::Create()
    {
        return new FVulkanSwapChain();
    }

    void FVulkanSwapChain::Present()
    {
        
    }
    
    void FVulkanSwapChain::Init(FWindow* InWindow)
    {
        Window = InWindow;

        /* Create window surface */
        VK_CHECK(glfwCreateWindowSurface(Vulkan::GetInstance(), Window->GetWindow(), nullptr, &Surface));

        /* Vulkan 1.3 Enabled Features */
        VkPhysicalDeviceVulkan13Features Features = {};
        Features.dynamicRendering = true;
        Features.synchronization2 = true;

        /* Vulkan 1.2 Enabled Features */
        VkPhysicalDeviceVulkan12Features Features12 = {};
        Features12.bufferDeviceAddress = true;
        Features12.descriptorIndexing = true;

        /* Select Vulkan devices */
        vkb::Instance vkb_inst = Vulkan::GetInstance();
        vkb::PhysicalDeviceSelector Selector{ vkb_inst };
        vkb::PhysicalDevice PhysicalDevice = Selector
        .set_minimum_version(1, 3)
        .set_required_features_13(Features)
        .set_required_features_12(Features12)
        .set_surface(Surface)
        .select()
        .value();

        /* Set Vulkan devices */
        vkb::DeviceBuilder DeviceBuilder { PhysicalDevice };
        vkb::Device Device = DeviceBuilder.build().value();

        
        /* Create Swap Chain */
        ImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        vkb::SwapchainBuilder SwapchainBuilder{ PhysicalDevice, Device, Surface };

        vkb::Swapchain vkbSwapChain = SwapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR { .format = ImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(InWindow->GetWidth(), InWindow->GetHeight())
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

        
        /* Send information to render context */
        FVulkanRenderContext* VkRenderContext = FApplication::Get().GetRenderContext<FVulkanRenderContext>();
        VkRenderContext->SetSwapChain(SwapChain);
        
        VkRenderContext->InitializedDependencies(Device, PhysicalDevice);
        
        bInitialized = true;
    }
    
}
