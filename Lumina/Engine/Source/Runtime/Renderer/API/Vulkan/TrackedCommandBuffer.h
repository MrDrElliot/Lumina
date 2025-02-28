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
            
        }

        ~FTrackedCommandBufer()
        {
            vkDestroyCommandPool(Device->GetDevice(), CommandPool, nullptr);
        }

        VkCommandBuffer CommandBuffer;
        VkCommandPool   CommandPool;
        
        TVector<TRefCountPtr<IRHIResource>> ReferencedResources;
        
        
    };
}
