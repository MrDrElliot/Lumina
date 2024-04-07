#include "VulkanDescriptor.h"

#include "VulkanHelpers.h"

namespace Lumina
{
    void FDescriptorAllocatorGrowable::Init(VkDevice InDevice, uint32_t InitialSets, std::span<FPoolSizeRatio> PoolRatios)
    {
        Ratios.clear();

        for(auto r : PoolRatios)
        {
            Ratios.push_back(r);
        }

        VkDescriptorPool NewPool = CreatePool(InDevice, InitialSets, PoolRatios);

        SetsPerPool = InitialSets * 1.5;

        ReadyPools.push_back(NewPool);
    }

    void FDescriptorAllocatorGrowable::ClearPools(VkDevice InDevice)
    {
        for (auto pool : ReadyPools)
        {
            vkResetDescriptorPool(InDevice, pool, 0);
        }
        for(auto pool : FullPools)
        {
            vkResetDescriptorPool(InDevice, pool, 0);
        }
        FullPools.clear();
    }

    void FDescriptorAllocatorGrowable::DestroyPools(VkDevice InDevice)
    {
        for(auto pool : ReadyPools)
        {
            vkDestroyDescriptorPool(InDevice, pool, nullptr);
        }
        for(auto pool : FullPools)
        {
            vkDestroyDescriptorPool(InDevice, pool, nullptr);
        }
        FullPools.clear();
    }

    VkDescriptorSet FDescriptorAllocatorGrowable::Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout)
    {
        VkDescriptorPool poolToUse = GetPool(InDevice);

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.pNext = nullptr;
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = poolToUse;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &InLayout;

        VkDescriptorSet ds;
        VkResult result = vkAllocateDescriptorSets(InDevice, &allocInfo, &ds);

        //allocation failed. Try again
        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {

            FullPools.push_back(poolToUse);
    
            poolToUse = GetPool(InDevice);
            allocInfo.descriptorPool = poolToUse;

            VK_CHECK( vkAllocateDescriptorSets(InDevice, &allocInfo, &ds));
        }
  
        ReadyPools.push_back(poolToUse);
        return ds;
    }

    VkDescriptorPool FDescriptorAllocatorGrowable::GetPool(VkDevice InDevice)
    {
        VkDescriptorPool NewPool;
        if(ReadyPools.size() != 0)
        {
            NewPool = ReadyPools.back();
            ReadyPools.pop_back();
        }
        else
        {
            NewPool = CreatePool(InDevice, SetsPerPool, Ratios);

            SetsPerPool = SetsPerPool * 1.5;
            if(SetsPerPool > 4092)
            {
                SetsPerPool = 4092;
            }
        }

        return NewPool;
    }

    VkDescriptorPool FDescriptorAllocatorGrowable::CreatePool(VkDevice InDevice, uint32_t SetCount, std::span<FPoolSizeRatio> PoolRatios)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        for (FPoolSizeRatio ratio : PoolRatios)
        {
            poolSizes.push_back(VkDescriptorPoolSize
            {
                .type = ratio.Type,
                .descriptorCount = uint32_t(ratio.Ratio * SetCount)
            });
        }

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = SetCount;
        pool_info.poolSizeCount = (uint32_t)poolSizes.size();
        pool_info.pPoolSizes = poolSizes.data();

        VkDescriptorPool newPool;
        vkCreateDescriptorPool(InDevice, &pool_info, nullptr, &newPool);
        return newPool;
    }

    void FDescriptorWriter::WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type)
    {
        VkDescriptorImageInfo& info = ImageInfos.emplace_back(VkDescriptorImageInfo
        {
            .sampler = sampler,
            .imageView = image,
            .imageLayout = layout
        });

        VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };

        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE; //left empty for now until we need to write it
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = &info;

        Writes.push_back(write);
    }

    void FDescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type)
    {
        VkDescriptorBufferInfo& info = BufferInfos.emplace_back(VkDescriptorBufferInfo
        {
            .buffer = buffer,
            .offset = offset,
            .range = size
        });

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE; //left empty for now until we need to write it
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = &info;

        Writes.push_back(write);
    }

    void FDescriptorWriter::Clear()
    {
        ImageInfos.clear();
        Writes.clear();
        BufferInfos.clear();
    }

    void FDescriptorWriter::UpdateSet(VkDevice device, VkDescriptorSet set)
    {
        for(VkWriteDescriptorSet& Write : Writes)
        {
            Write.dstSet = set;
        }

        vkUpdateDescriptorSets(device, (uint32_t)Writes.size(), Writes.data(), 0, nullptr);
    }

    void FDescriptorLayoutBuilder::AddBinding(uint32_t InBinding, VkDescriptorType InType)
    {
        VkDescriptorSetLayoutBinding Binding = {};
        Binding.binding = InBinding;
        Binding.descriptorCount = 1;
        Binding.descriptorType = InType;

        Bindings.push_back(Binding);
    }

    void FDescriptorLayoutBuilder::Clear()
    {
        Bindings.clear();
    }

    VkDescriptorSetLayout FDescriptorLayoutBuilder::Build(VkDevice InDevice, VkShaderStageFlags InShaderStages)
    {
       for(auto& Binding : Bindings)
       {
           Binding.stageFlags |= InShaderStages;
       }

        VkDescriptorSetLayoutCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        Info.pNext = nullptr;
        Info.pBindings = Bindings.data();
        Info.bindingCount = static_cast<uint32_t>(Bindings.size());
        Info.flags = 0;

        VkDescriptorSetLayout Set;

        VK_CHECK(vkCreateDescriptorSetLayout(InDevice, &Info, nullptr, &Set));

        return Set;
    }

    void FDescriptorAllocator::InitPool(VkDevice InDevice, uint32_t MaxSets, std::span<FPoolSizeRatio> PoolRatios)
    {
        std::vector<VkDescriptorPoolSize> PoolSizes;

        for(auto Ratio : PoolRatios)
        {
            PoolSizes.push_back(VkDescriptorPoolSize { .type = Ratio.Type, .descriptorCount = static_cast<uint32_t>(Ratio.Ratio * MaxSets) });
        }

        VkDescriptorPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.pNext = nullptr;
        PoolInfo.flags = 0;
        PoolInfo.maxSets = MaxSets;
        PoolInfo.poolSizeCount = static_cast<uint32_t>(PoolSizes.size());
        PoolInfo.pPoolSizes = PoolSizes.data();

        vkCreateDescriptorPool(InDevice, &PoolInfo, nullptr, &Pool);
    }
    

    void FDescriptorAllocator::ClearDescriptors(VkDevice InDevice)
    {
        vkResetDescriptorPool(InDevice, Pool, 0);
    }

    void FDescriptorAllocator::DestroyPool(VkDevice InDevice)
    {
        vkDestroyDescriptorPool(InDevice, Pool, nullptr);
    }

    VkDescriptorSet FDescriptorAllocator::Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout)
    {
        VkDescriptorSetAllocateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        Info.pNext = nullptr;
        Info.descriptorPool = Pool;
        Info.descriptorSetCount = 1;
        Info.pSetLayouts = &InLayout;

        VkDescriptorSet Set;
        VK_CHECK(vkAllocateDescriptorSets(InDevice, &Info, &Set));

        return Set;
    }
}
