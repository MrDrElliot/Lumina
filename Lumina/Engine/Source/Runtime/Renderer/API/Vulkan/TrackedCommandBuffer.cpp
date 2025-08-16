#include "TrackedCommandBuffer.h"

#include "VulkanRenderContext.h"


namespace Lumina
{
    FTrackedCommandBuffer::FTrackedCommandBuffer(FVulkanDevice* InDevice, VkCommandBuffer InBuffer, VkCommandPool InPool, bool bCreateTraceContext, VkQueue InQueue)
        : IDeviceChild(InDevice)
        , CommandBuffer(InBuffer)
        , CommandPool(InPool)
        , Queue(InQueue)
    {
        if (bCreateTraceContext)
        {
            TracyContext = TracyVkContext(InDevice->GetPhysicalDevice(),
                InDevice->GetDevice(),
                InQueue,
                CommandBuffer)
        }
    }

    FTrackedCommandBuffer::~FTrackedCommandBuffer()
    {
        if (TracyContext)
        {
            TracyVkDestroy(TracyContext)
        }
        vkDestroyCommandPool(Device->GetDevice(), CommandPool, VK_ALLOC_CALLBACK);
    }

    void FTrackedCommandBuffer::AddReferencedResource(const TRefCountPtr<IRHIResource>& InResource)
    {
        ReferencedResources.push_back(InResource);
    }

    void FTrackedCommandBuffer::AddStagingResource(const TRefCountPtr<FRHIBuffer>& InResource)
    {
        ReferencedStagingResources.push_back(InResource);
    }

    void FTrackedCommandBuffer::ClearReferencedResources()
    {
        ReferencedResources.clear();
        ReferencedStagingResources.clear();
    }
}
