#pragma once

#include <deque>
#include <functional>

#include "vk_mem_alloc.h"


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
    

    
}
