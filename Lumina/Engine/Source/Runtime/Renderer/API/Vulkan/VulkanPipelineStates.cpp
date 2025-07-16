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

        // Check if PendingStreams differs from LastPendingStreams
        bool bShouldRebind = std::memcmp(PendingStreams, LastPendingStreams, sizeof(PendingStreams)) != 0;
        if (!bShouldRebind)
        {
            return;
        }
        
        for (uint32 i = 0; i < 17; ++i)
        {
            const FVertexStream& Curr = PendingStreams[i];

            if (Curr.Stream != VK_NULL_HANDLE)
            {
                Buffers[NumBindings] = Curr.Stream;
                Offsets[NumBindings] = Curr.BufferOffset;
                NumBindings++;
            }
        }

        // If anything has changed, bind the buffers and copy PendingStreams to LastPendingStreams
        if (bShouldRebind && NumBindings > 0)
        {
            vkCmdBindVertexBuffers(CmdBuffer, 0, NumBindings, Buffers, Offsets);

            // Efficiently copy PendingStreams to LastPendingStreams
            std::memcpy(LastPendingStreams, PendingStreams, sizeof(PendingStreams));
        }
    }
}
