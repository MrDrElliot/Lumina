#pragma once

#include <vulkan/vulkan_core.h>

#include "Containers/Array.h"
#include "Renderer/CommandList.h"

namespace Lumina
{
    class FVulkanRenderContext;
}

namespace Lumina
{
    class FVulkanCommandList : public ICommandList
    {
    public:

        void Begin() override;
        void FlushCommandList() override;
        void SubmitCommandList() override;
        void Reset() override;
        void Destroy() override;

        FVulkanRenderContext* RenderContext;
        VkCommandPool   Pool;
        VkQueue         Queue;
        VkCommandBuffer CommandBuffer;
        VkFence Fence;
        TVector<VkSemaphore> WaitSemaphores;
        TVector<VkSemaphore> SignalSemaphores;
        
    };
}
