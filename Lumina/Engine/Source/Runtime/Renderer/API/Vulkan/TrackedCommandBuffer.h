#pragma once
#include "VulkanDevice.h"
#include "Memory/RefCounted.h"
#include "Renderer/RenderResource.h"

namespace Lumina
{
    class FTrackedCommandBufer : public FRefCounted, public IDeviceChild
    {
    public:

        FTrackedCommandBufer(FVulkanDevice* InDevice, VkCommandBuffer InBuffer, VkCommandPool InPool)
            : IDeviceChild(InDevice)
            , CommandBuffer(InBuffer)
            , CommandPool(InPool)
        {
            ReferencedResources.reserve(24);
        }

        ~FTrackedCommandBufer()
        {
            vkDestroyCommandPool(Device->GetDevice(), CommandPool, nullptr);
        }

        void AddReferencedResource(TRefCountPtr<IRHIResource> InResource)
        {
            ReferencedResources.push_back(InResource);
        }

        void ClearReferencedResources()
        {
            SIZE_T LastSize = ReferencedResources.size();
            ReferencedResources.clear();
            ReferencedResources.reserve(LastSize);
        }

        VkCommandBuffer CommandBuffer;
        VkCommandPool   CommandPool;

        /** Here we keep alive any resources that this current command buffer needs/uses */
        TVector<TRefCountPtr<IRHIResource>> ReferencedResources;
        
    };
}
