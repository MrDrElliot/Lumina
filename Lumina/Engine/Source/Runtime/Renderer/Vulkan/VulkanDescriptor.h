#pragma once
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Lumina
{
    struct FDescriptorLayoutBuilder
    {
        std::vector<VkDescriptorSetLayoutBinding> Bindings;

        void AddBinding(uint32_t InBinding, VkDescriptorType InType);
        void Clear();
        VkDescriptorSetLayout Build(VkDevice InDevice, VkShaderStageFlags InShaderStages);
    };


    struct FDescriptorAllocator
    {

        struct FPoolSizeRatio
        {
            VkDescriptorType Type;
            float Ratio;
        };


        VkDescriptorPool Pool;

        void InitPool(VkDevice InDevice, uint32_t MaxSets, std::span<FPoolSizeRatio> PoolRatios);
        void ClearDescriptors(VkDevice InDevice);
        void DestroyPool(VkDevice InDevice);


        VkDescriptorSet Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout);
        
    };
    
}
