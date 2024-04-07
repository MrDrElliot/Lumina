#pragma once
#include <deque>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Lumina
{

    struct FDescriptorAllocatorGrowable
    {
        struct FPoolSizeRatio
        {
            VkDescriptorType Type;
            float Ratio;
        };

        void Init(VkDevice InDevice, uint32_t InitialSets, std::span<FPoolSizeRatio> PoolRatios);
        void ClearPools(VkDevice InDevice);
        void DestroyPools(VkDevice InDevice);

        VkDescriptorSet Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout);

    private:

        VkDescriptorPool GetPool(VkDevice InDevice);
        VkDescriptorPool CreatePool(VkDevice InDevice, uint32_t SetCount, std::span<FPoolSizeRatio> PoolRatios);

        std::vector<FPoolSizeRatio> Ratios;
        std::vector<VkDescriptorPool> FullPools;
        std::vector<VkDescriptorPool> ReadyPools;
        uint32_t SetsPerPool;
    };

    struct FDescriptorWriter
    {

        void WriteImage(int binding,VkImageView image,VkSampler sampler , VkImageLayout layout, VkDescriptorType type);
        void WriteBuffer(int binding,VkBuffer buffer,size_t size, size_t offset,VkDescriptorType type); 

        void Clear();
        void UpdateSet(VkDevice device, VkDescriptorSet set);

        std::deque<VkDescriptorImageInfo> ImageInfos;
        std::deque<VkDescriptorBufferInfo> BufferInfos;
        std::vector<VkWriteDescriptorSet> Writes;
    };
    
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
