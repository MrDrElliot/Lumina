#pragma once
#include <shared_mutex>

#include "Source/Runtime/Renderer/CommandBuffer.h"
#include "vulkanmemoryallocator/src/Common.h"

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
        
        VkCommandBuffer& GetCommandBuffer() { return CommandBuffer; }

    private:
        
        VkCommandBuffer CommandBuffer;

        inline static std::shared_mutex SubmissionMutex;
    
    };
}
