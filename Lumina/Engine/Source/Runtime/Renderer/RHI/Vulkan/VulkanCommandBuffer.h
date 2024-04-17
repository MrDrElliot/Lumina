#pragma once
#include "Source/Runtime/Renderer/CommandBuffer.h"
#include "vulkanmemoryallocator/src/Common.h"

namespace Lumina
{
    class FVulkanCommandBuffer : public FCommandBuffer
    {
    public:

        FVulkanCommandBuffer();
        FVulkanCommandBuffer(bool bTransient);
        ~FVulkanCommandBuffer();
        
        void Begin() override;
        void End() override;
        void Reset() override;
        void Execute() override;

        void Destroy() override;

        VkCommandBuffer& GetCommandBuffer() { return CommandBuffer; }
        VkCommandPool& GetCommandPool() { return CommandPool; }

    private:


        VkCommandBuffer CommandBuffer;
        VkCommandPool CommandPool;
    
    };
}
