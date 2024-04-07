#pragma once

#include <deque>
#include <functional>
#include "glm/glm.hpp"
#include <VulkanMemoryAllocator/include/vk_mem_alloc.h>

namespace Lumina
{
    
/* Max number of frames to overlap */
constexpr unsigned int FRAME_OVERLAP = 2;
    
    struct FDeletionQueue
    {
        std::deque<std::function<void()>> Deletors;

        template<typename F>
        void Add(F&& function)
        {
            static_assert(sizeof(F) < 200, "DONT CAPTURE TOO MUCH IN THE LAMBDA");
            Deletors.push_back(function);
        }

        void Flush()
        {
            for (auto it = Deletors.rbegin(); it != Deletors.rend(); it++)
            {
                (*it)();
            }

            Deletors.clear();
        }
    };
    
    struct FVertex
    {
        glm::vec3 Position;
        float UV_x;
        glm::vec3 Normal;
        float UV_y;
        glm::vec4 Color;
    };

    struct FAllocatedBuffer
    {
        VkBuffer Buffer;
        VmaAllocation Allocation;
        VmaAllocationInfo Info;
    };
    
    struct FGPUMeshBuffers
    {
        FAllocatedBuffer IndexBuffer;
        FAllocatedBuffer VertexBuffer;
        VkDeviceAddress VertexBufferAddress;
    };

    struct FGPUDrawPushConstants
    {
        glm::mat4 WorldMatrix;
        VkDeviceAddress VertexBuffer;
    };
    
    struct FAllocatedImage
    {
        VkImage Image;
        VkImageView ImageView;
        VmaAllocation Allocation;
        VkExtent3D ImageExtent;
        VkFormat ImageFormat;
    };


    struct FFrameData
    {
        VkSemaphore SwapchainSemaphore;
        VkSemaphore RenderSemaphore;

        VkFence RenderFence;
        
        VkCommandBuffer CommandBuffer;
        VkCommandPool CommandPool;

        FDeletionQueue DeletionQueue;
    };

    
    struct FComputePushConstants
    {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

    struct FComputeEffect
    {
        const char* Name;
        VkPipeline Pipeline;
        VkPipelineLayout Layout;

        FComputePushConstants Data;
    };
    
    
}
