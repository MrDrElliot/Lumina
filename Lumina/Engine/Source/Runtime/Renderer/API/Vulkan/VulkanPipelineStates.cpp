#include "VulkanPipelineStates.h"

namespace Lumina
{
    void FVulkanGraphicsState::SetVertexStream(FRHIBuffer* Buffer, uint32 Index, uint32 Offset)
    {
        PendingStreams[Index].Stream = Buffer->GetAPIResource<VkBuffer>();
        PendingStreams[Index].BufferOffset = Offset;
    }

    void FVulkanGraphicsState::PrepareForDraw(VkCommandBuffer CmdBuffer)
    {
        VkBuffer Buffers[17];
        VkDeviceSize Offsets[17];
        uint32 NumBindings = 0;

        for (uint32 i = 0; i < 17; ++i)
        {
            if (PendingStreams[i].Stream != VK_NULL_HANDLE)
            {
                Buffers[NumBindings] = PendingStreams[i].Stream;
                Offsets[NumBindings] = PendingStreams[i].BufferOffset;
                NumBindings++;
            }
        }

        if (NumBindings > 0)
        {
            vkCmdBindVertexBuffers(CmdBuffer, 0, NumBindings, Buffers, Offsets);
        }
    }

}
