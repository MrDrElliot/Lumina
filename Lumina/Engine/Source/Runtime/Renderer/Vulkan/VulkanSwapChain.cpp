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

    VkExtent2D& FVulkanSwapChain::GetDrawExtent2D()
    {
        VkExtent2D Extent2D =  { DrawImage.ImageExtent.width, DrawImage.ImageExtent.height };

        return Extent2D;
    }

    VkExtent3D& FVulkanSwapChain::GetExtent()
    {
        VkExtent3D Extent3D;
        Extent3D.width = SwapChain.extent.width;
        Extent3D.height = SwapChain.extent.height;
        Extent3D.depth = 1;

        return Extent3D;
    }
    
    void FVulkanSwapChain::Init(FWindow* InWindow)
    {
        Window = InWindow;
        
        FVulkanRenderContext* VkRenderContext = FApplication::Get().GetRenderContext<FVulkanRenderContext>();

        /* Send information to render context */
        VkRenderContext->SetSwapChain(this);

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

        SwapChain = SwapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR { .format = ImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(InWindow->GetWidth(), InWindow->GetHeight())
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();
        

        DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        DrawImage.ImageExtent = GetExtent();

        VkImageUsageFlags DrawImageFlags = {};
        DrawImageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo ImgInfo = Vulkan::ImageCreateInfo(DrawImage.ImageFormat, DrawImageFlags, GetExtent());

        VmaAllocationCreateInfo AllocationInfo  = {};
        AllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        AllocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkRenderContext->InitializedDependencies(Device, PhysicalDevice);

        /* Allocate and create the draw image */
        vmaCreateImage(VkRenderContext->GetAllocator(), &ImgInfo, &AllocationInfo, &DrawImage.Image, &DrawImage.Allocation, nullptr);

        VkImageViewCreateInfo ViewInfo = Vulkan::ImageViewCreateInfo(DrawImage.ImageFormat, DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_CHECK(vkCreateImageView(Device, &ViewInfo, nullptr, &DrawImage.ImageView));
        

        VkRenderContext->FinalInitialization();
        
        bInitialized = true;
    }
    
}
