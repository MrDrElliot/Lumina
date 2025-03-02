#include "VulkanResources.h"

#include "VulkanCommandList.h"
#include "VulkanMacros.h"

namespace Lumina
{

    VkBufferUsageFlags ToVkBufferUsage(TBitFlags<EBufferUsageFlags> Usage) 
    {
        VkBufferUsageFlags result = 0;

        if (Usage.IsFlagSet(EBufferUsageFlags::VertexBuffer))
        {
            result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
    
        if (Usage.IsFlagSet(EBufferUsageFlags::IndexBuffer))
        {
            result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
    
        if (Usage.IsFlagSet(EBufferUsageFlags::UniformBuffer))
        {
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::SourceCopy))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::StagingBuffer))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::CPUWritable))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        
        return result;
    }
    
    FVulkanBuffer::FVulkanBuffer(FVulkanDevice* InDevice, const FRHIBufferDesc& InDescription)
        : FRHIBuffer(InDescription)
        , IDeviceChild(InDevice)
    {
        VmaAllocationCreateFlags VmaFlags = 0;

        if(GetDescription().Usage.IsFlagSet(EBufferUsageFlags::CPUWritable))
        {
            VmaFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        
        
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = InDescription.Size;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = ToVkBufferUsage(InDescription.Usage);
        BufferCreateInfo.flags = 0;
        
        Device->GetAllocator()->AllocateBuffer(&BufferCreateInfo, VmaFlags, &Buffer, "");
        
        Assert(Buffer != VK_NULL_HANDLE);
    }

    
    //----------------------------------------------------------------------------------------------
    

    FVulkanImage::FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription)
        : FRHIImage(InDescription)
        , IDeviceChild(InDevice)
    {
        
        VkImageCreateFlags ImageFlags = VK_NO_FLAGS;
        
        
        VkImageCreateInfo ImageCreateInfo = {};
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageCreateInfo.extent = VkExtent3D(GetExtent().X, GetExtent().Y, 1);
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.flags = ImageFlags;
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.mipLevels = 1;
        ImageCreateInfo.arrayLayers = 1;
        ImageCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        FullAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        PartialAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        
        VmaAllocationCreateFlags AllocationFlags = VK_NO_FLAGS;
        Device->GetAllocator()->AllocateImage(&ImageCreateInfo, AllocationFlags, &Image, "");

        Assert(Image != VK_NULL_HANDLE);


        // Create Image View
        VkImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = Image;
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;

        VK_CHECK(vkCreateImageView(Device->GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));

        Assert(ImageView != VK_NULL_HANDLE);

    }

    FVulkanImage::FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription, VkImage RawImage, VkImageView RawView)
        : FRHIImage(InDescription)
        , IDeviceChild(InDevice)
        , Image(RawImage)
        , ImageView(RawView)
    {
        FullAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        PartialAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    FVulkanImage::~FVulkanImage()
    {
        vkDestroyImageView(Device->GetDevice(), ImageView, nullptr);
    }

    void* FVulkanImage::GetAPIResourceImpl(EAPIResourceType Type)
    {
        switch (Type)
        {
            case EAPIResourceType::Image: return Image;
            case EAPIResourceType::ImageView: return ImageView;
        }

        return nullptr;
    }
}
