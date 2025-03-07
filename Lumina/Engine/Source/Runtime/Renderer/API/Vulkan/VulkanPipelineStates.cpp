#include "VulkanPipelineStates.h"

namespace Lumina
{
    void FVulkanGraphicsState::SetVertexStream(FRHIBuffer* Buffer, uint32 Index, uint32 Offset)
    {
        PendingStreams[Index].Stream = static_cast<FVulkanBuffer*>(Buffer)->GetBuffer();
        PendingStreams[Index].BufferOffset = Offset;
    }

    void FVulkanGraphicsState::PrepareForDraw(VkCommandBuffer CmdBuffer)
    {
        
    }
}
