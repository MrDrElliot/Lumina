#include "TrackedCommandBuffer.h"

#include "VulkanRenderContext.h"


namespace Lumina
{
    FTrackedCommandBuffer::~FTrackedCommandBuffer()
    {
        if (TracyContext)
        {
            TracyVkDestroy(TracyContext)
        }
        vkDestroyCommandPool(Device->GetDevice(), CommandPool, VK_ALLOC_CALLBACK);
    }
}
