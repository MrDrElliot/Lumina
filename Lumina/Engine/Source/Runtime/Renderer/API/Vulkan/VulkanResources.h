#pragma once

#include "VulkanDevice.h"
#include "Renderer/RenderResource.h"

namespace Lumina
{

    class FVulkanBuffer : public FRHIBuffer, public IDeviceChild
    {
    public:

        FVulkanBuffer(FVulkanDevice* InDevice, const FRHIBufferDesc& InDescription);


        FORCEINLINE VkBuffer GetBuffer() const { return Buffer; }
        

    private:
        
        VkBuffer Buffer = VK_NULL_HANDLE;
    };

    
    //----------------------------------------------------------------------------------------------


    class FVulkanImage : public FRHIImage, public IDeviceChild
    {
    public:

        FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription);
        FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription, VkImage RawImage, VkImageView RawView);
        ~FVulkanImage() override;

        VkImage GetImage() const { return Image; }
        VkImageView GetImageView() const { return ImageView; }

        FORCEINLINE void SetDefaultLayout(VkImageLayout InDefault) { DefaultLayout = InDefault; }
        FORCEINLINE VkImageLayout GetDefaultLayout() const { return DefaultLayout; }

        FORCEINLINE VkImageAspectFlags GetFullAspectMask() const { return FullAspectMask; }
        FORCEINLINE VkImageAspectFlags GetPartialAspectMask() const { return PartialAspectMask; }

    private:
        
        VkImageLayout               DefaultLayout =     VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageAspectFlags          FullAspectMask =    VK_IMAGE_ASPECT_NONE;
        VkImageAspectFlags          PartialAspectMask = VK_IMAGE_ASPECT_NONE;
        VkImage                     Image =             VK_NULL_HANDLE;
        VkImageView                 ImageView =         VK_NULL_HANDLE;
    };
    
    
}
