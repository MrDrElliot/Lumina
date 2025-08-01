﻿#pragma once
#include "VulkanResources.h"
#include "Renderer/RenderTypes.h"



namespace Lumina
{
    class FVulkanGraphicsState
    {
    public:

        FVulkanGraphicsState() = default;

        void SetVertexStream(FRHIBuffer* Buffer, uint32 Index, uint32 Offset);
        void PrepareForDraw(VkCommandBuffer CmdBuffer);
        
        struct FVertexStream
        {
            FVertexStream()
                : Stream(VK_NULL_HANDLE)
                , BufferOffset(0)
            {}

            VkBuffer Stream;
            uint32 BufferOffset;
        };

        FVertexStream                           LastPendingStreams[17];
        VkBuffer                                IndexBuffer = nullptr;
        FVertexStream                           PendingStreams[17];
        FVulkanGraphicsPipeline*                Pipeline = nullptr;
        TVector<TPair<FRHIBindingSet*, uint32>> CurrentBindings;
        FRenderPassBeginInfo                    CurrentRenderPassInfo;
    };

    class FVulkanComputeState
    {
    public:

        FVulkanComputeState() = default;
        
        FVulkanComputePipeline*                 Pipeline;
        TVector<TPair<FRHIBindingSet*, uint32>> CurrentBindings;

    };
    
}
