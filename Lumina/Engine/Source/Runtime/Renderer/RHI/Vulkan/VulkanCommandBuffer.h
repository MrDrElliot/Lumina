#pragma once
#include <shared_mutex>
#include "VulkanBuffer.h"
#include "Source/Runtime/Renderer/CommandBuffer.h"

namespace Lumina
{
    class FVulkanCommandBuffer : public FCommandBuffer
    {
    public:

        FVulkanCommandBuffer(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType);
        ~FVulkanCommandBuffer() override;

        void SetFriendlyName(const FString& InName) override;
        
        void Begin() override;
        void End() override;
        void Reset() override;
        void Execute(bool bWait = false) override;
        
        FORCEINLINE VkCommandBuffer& GetCommandBuffer() { Assert(CommandBuffer); return CommandBuffer; }

    private:
        
        VkCommandBuffer CommandBuffer;
        
    };
}
