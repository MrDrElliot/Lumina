#include "VulkanSwapChain.h"

#include <vk-bootstrap/src/VkBootstrap.h>
#include "VulkanHelpers.h"
#include "VulkanRenderContext.h"
#include "backends/imgui_impl_vulkan.h"
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

    void FVulkanSwapChain::Resize(uint32_t InWidth, uint32_t InHeight)
    {
        if(Device && VkRenderContext)
        {
            vkDeviceWaitIdle(Device);

            DestroySwapchain();
            
            CreateSwapChain(InWidth, InHeight);

            VkExtent3D Extent = GetExtent();
            DrawImage.ImageExtent = Extent;
            DepthImage.ImageExtent = Extent;
            
            bResizeRequested = false;
        }
    }

    void FVulkanSwapChain::DestroySwapchain()
    {
        std::vector<VkImageView> Images = GetImageViews();
        vkDestroySwapchainKHR(Device, SwapChain, nullptr);
        
        for(auto Image : Images)
        {
            vkDestroyImageView(Device, Image, nullptr);
        }
    }

    void FVulkanSwapChain::CreateSwapChain(uint32_t NewWidth, uint32_t NewHeight)
    {
        ImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        
        vkb::SwapchainBuilder SwapchainBuilder{ Device.physical_device, Device, Surface };

        SwapChain = SwapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR { .format = ImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(NewWidth, NewHeight)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

        

        /* Send information to render context */
        VkRenderContext->SetSwapChain(this);
    }

    void FVulkanSwapChain::CreateDevices()
    {

        /* Vulkan 1.3 Enabled Features */
        VkPhysicalDeviceVulkan13Features Features = {};
        Features.dynamicRendering = VK_TRUE;
        Features.synchronization2 = VK_TRUE;

        /* Vulkan 1.2 Enabled Features */
        VkPhysicalDeviceVulkan12Features Features12 = {};
        Features12.bufferDeviceAddress = VK_TRUE;
        Features12.descriptorIndexing = VK_TRUE;

        
        /* Select Vulkan devices */
        vkb::Instance vkb_inst = Vulkan::GetInstance();
        vkb::PhysicalDeviceSelector Selector{ vkb_inst };
        vkb::PhysicalDevice PhysicalDevice = Selector
        .add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
        .set_minimum_version(1, 3)
        .set_required_features_13(Features)
        .set_required_features_12(Features12)
        .set_surface(Surface)
        .select()
        .value();

        /* Set Vulkan devices */
        vkb::DeviceBuilder DeviceBuilder { PhysicalDevice };
        Device = DeviceBuilder.build().value();
    }

    VkExtent3D FVulkanSwapChain::GetDrawExtent()
    {
        VkExtent3D Extent3D;
        Extent3D.width =  std::min(GetExtent().width, DrawImage.ImageExtent.width) * RenderScale;
        Extent3D.height = std::min(GetExtent().height, DrawImage.ImageExtent.height) * RenderScale;
        Extent3D.depth = 1;
        
        return Extent3D;
    }

    VkExtent2D FVulkanSwapChain::GetDrawExtent2D()
    {
        VkExtent2D Extent2D;
        Extent2D.width = GetDrawExtent().width;
        Extent2D.height= GetDrawExtent().height;

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
        
        VkRenderContext = FApplication::Get().GetRenderContext<FVulkanRenderContext>();

        /* Send information to render context */
        VkRenderContext->SetSwapChain(this);

        /* Create window surface */
        VK_CHECK(glfwCreateWindowSurface(Vulkan::GetInstance(), Window->GetWindow(), nullptr, &Surface));

        CreateDevices();
        
        CreateSwapChain(InWindow->GetWidth(), InWindow->GetHeight());

        DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        DrawImage.ImageExtent = GetExtent();

        DepthImage.ImageFormat = VK_FORMAT_D32_SFLOAT;
        DepthImage.ImageExtent = GetExtent();

        
        VkImageUsageFlags DepthImageFlags = {};
        DepthImageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        

        VkImageUsageFlags DrawImageFlags = {};
        DrawImageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        DrawImageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        

        VkImageCreateInfo ImgInfo = Vulkan::ImageCreateInfo(DrawImage.ImageFormat, DrawImageFlags, GetExtent());

        VmaAllocationCreateInfo AllocationInfo  = {};
        AllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        AllocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkRenderContext->InitializedDependencies(Device, Device.physical_device);

        /* Allocate and create the draw image */
        vmaCreateImage(VkRenderContext->GetAllocator(), &ImgInfo, &AllocationInfo, &DrawImage.Image, &DrawImage.Allocation, nullptr);
        

        /* Allocate and create the depth image */
        VkImageCreateInfo DepthImgInfo = Vulkan::ImageCreateInfo(DepthImage.ImageFormat, DepthImageFlags, GetExtent());
        vmaCreateImage(VkRenderContext->GetAllocator(), &DepthImgInfo, &AllocationInfo, &DepthImage.Image, &DepthImage.Allocation, nullptr);

        VkImageViewCreateInfo DepthViewInfo = Vulkan::ImageViewCreateInfo(DepthImage.ImageFormat, DepthImage.Image, VK_IMAGE_ASPECT_DEPTH_BIT);
        VkImageViewCreateInfo ViewInfo = Vulkan::ImageViewCreateInfo(DrawImage.ImageFormat, DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);


        VK_CHECK(vkCreateImageView(Device, &DepthViewInfo, nullptr, &DepthImage.ImageView));
        VK_CHECK(vkCreateImageView(Device, &ViewInfo, nullptr, &DrawImage.ImageView));
        

        VkRenderContext->FinalInitialization();
        
        bInitialized = true;
    }

}
