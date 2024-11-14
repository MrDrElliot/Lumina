#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanRenderAPI.h"
#include "VulkanRenderContext.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"

namespace Lumina
{

    constexpr VkDescriptorType UsageToDescriptorType(const EDeviceBufferUsage& usage)
    {
        switch (usage)
        {
        case EDeviceBufferUsage::UNIFORM_BUFFER:		    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case EDeviceBufferUsage::UNIFORM_BUFFER_DYNAMIC:    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case EDeviceBufferUsage::STORAGE_BUFFER:		    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case EDeviceBufferUsage::STORAGE_BUFFER_DYNAMIC:    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        default:									    std::unreachable();
        }
    }

    constexpr VkDescriptorType convert(const EDescriptorBindingType& type)
    {
        switch (type)
        {
        case EDescriptorBindingType::SAMPLED_IMAGE:		        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case EDescriptorBindingType::STORAGE_IMAGE:		        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case EDescriptorBindingType::UNIFORM_BUFFER:	        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case EDescriptorBindingType::UNIFORM_BUFFER_DYNAMIC:	return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case EDescriptorBindingType::STORAGE_BUFFER:	        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case EDescriptorBindingType::STORAGE_BUFFER_DYNAMIC:    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        default:										        std::unreachable();
        }
    }

    const VkShaderStageFlags GetShaderStageFlags(EShaderStage stage)
    {
        VkShaderStageFlags flags = 0;

        if ((stage & EShaderStage::VERTEX) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        if ((stage & EShaderStage::FRAGMENT) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if ((stage & EShaderStage::COMPUTE) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_COMPUTE_BIT;
        }
        if ((stage & EShaderStage::GEOMETRY) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        }
        if ((stage & EShaderStage::TESSELLATION_CONTROL) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        }
        if ((stage & EShaderStage::TESSELLATION_EVALUATION) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }
        if ((stage & EShaderStage::ALL_GRAPHICS) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_ALL_GRAPHICS;
        }
        if ((stage & EShaderStage::ALL) != EShaderStage::NONE)
        {
            flags |= VK_SHADER_STAGE_ALL;
        }

        return flags;
    }
    
    constexpr VkDescriptorBindingFlags extractFlags(const uint64& Mask)
    {
        VkDescriptorBindingFlags Flags = 0;

        if (Mask & (uint32)EDescriptorFlags::PARTIALLY_BOUND)
        {
            Flags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
        }

        return Flags;
    }
    
    FVulkanDescriptorSet::FVulkanDescriptorSet(const FDescriptorSetSpecification& InSpec)
    : DescriptorSet(VK_NULL_HANDLE), Layout(VK_NULL_HANDLE)
    {
        auto Device = FVulkanRenderContext::GetDevice();

        TFastVector<VkDescriptorSetLayoutBinding> SetBindings;
        TFastVector<VkDescriptorBindingFlags> BindingFlags;

        for (auto& binding : InSpec.Bindings)
        {
            VkDescriptorSetLayoutBinding VkBinding = {};
            VkBinding.stageFlags = GetShaderStageFlags(binding.ShaderStage);
            VkBinding.pImmutableSamplers = nullptr;
            VkBinding.descriptorCount = binding.ArrayCount;
            VkBinding.descriptorType = convert(binding.Type);
            VkBinding.binding = binding.Binding;

            SetBindings.push_back(std::move(VkBinding));
            BindingFlags.push_back(extractFlags(binding.Flags));
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo VkBindingFlags = {};
        VkBindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        VkBindingFlags.bindingCount = (uint32)InSpec.Bindings.size();
        VkBindingFlags.pBindingFlags = BindingFlags.data();

        VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
        LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        LayoutCreateInfo.pNext = &VkBindingFlags;
        LayoutCreateInfo.bindingCount = (uint32)SetBindings.size();
        LayoutCreateInfo.pBindings = SetBindings.data();
        LayoutCreateInfo.flags = 0;

        vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &Layout);

        std::vector<VkDescriptorSet> set = FVulkanRenderAPI::AllocateDescriptorSets(Layout, 1);
        DescriptorSet = set[0];
    }

    FVulkanDescriptorSet::~FVulkanDescriptorSet()
    {
        
    }

    void FVulkanDescriptorSet::Destroy()
    {
        auto Device = FVulkanRenderContext::GetDevice();
        vkDestroyDescriptorSetLayout(Device, Layout, nullptr);
        FVulkanRenderAPI::FreeDescriptorSets({ DescriptorSet });
    }

    void FVulkanDescriptorSet::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset)
    {
        auto Device = FVulkanRenderContext::GetDevice();
        
        TRefPtr<FVulkanBuffer> vkBuffer = RefPtrCast<FVulkanBuffer>(Buffer);

        VkDescriptorBufferInfo DescriptorBufferInfo = {};
        DescriptorBufferInfo.buffer = vkBuffer->GetBuffer();
        DescriptorBufferInfo.range = Size;
        DescriptorBufferInfo.offset = Offset;

        VkWriteDescriptorSet WriteDescriptorSet = {};
        WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptorSet.dstSet = DescriptorSet;
        WriteDescriptorSet.dstBinding = Binding;
        WriteDescriptorSet.descriptorType = UsageToDescriptorType(vkBuffer->GetSpecification().BufferUsage);
        WriteDescriptorSet.dstArrayElement = ArrayElement;
        WriteDescriptorSet.descriptorCount = 1;
        WriteDescriptorSet.pBufferInfo = &DescriptorBufferInfo;
        

        vkUpdateDescriptorSets(Device, 1, &WriteDescriptorSet, 0, nullptr);
    }

    void FVulkanDescriptorSet::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler)
    {
        auto Device = FVulkanRenderContext::GetDevice();
        TRefPtr<FVulkanImage> vkImage = RefPtrCast<FVulkanImage>(Image);
        TRefPtr<FVulkanImageSampler> vk_sampler = RefPtrCast<FVulkanImageSampler>(Sampler);

        VkDescriptorImageInfo DescriptorImageInfo = {};
        DescriptorImageInfo.imageView = vkImage->GetImageView();
        DescriptorImageInfo.sampler = vk_sampler->GetSampler();
        DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet WriteDescriptorSet = {};
        WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptorSet.dstSet = DescriptorSet;
        WriteDescriptorSet.dstBinding = Binding;
        WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        WriteDescriptorSet.dstArrayElement = ArrayElement;
        WriteDescriptorSet.descriptorCount = 1;
        WriteDescriptorSet.pImageInfo = &DescriptorImageInfo;

        vkUpdateDescriptorSets(Device, 1, &WriteDescriptorSet, 0, nullptr);
    }

    void FVulkanDescriptorSet::Write(uint16 Binding, uint16 ArrayElement, TFastVector<TRefPtr<FImage>> Images, TRefPtr<FImageSampler> Sampler)
    {
        auto Device = FVulkanRenderContext::GetDevice();
    
        TFastVector<VkDescriptorImageInfo> DescriptorImageInfos(Images.size());

        for (size_t i = 0; i < Images.size(); ++i)
        {
            TRefPtr<FVulkanImage> vkImage = RefPtrCast<FVulkanImage>(Images[i]);
            TRefPtr<FVulkanImageSampler> vkSampler = RefPtrCast<FVulkanImageSampler>(Sampler);

            DescriptorImageInfos[i].imageView = vkImage->GetImageView();
            DescriptorImageInfos[i].sampler = vkSampler->GetSampler();
            DescriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        VkWriteDescriptorSet WriteDescriptorSet = {};
        WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptorSet.dstSet = DescriptorSet;
        WriteDescriptorSet.dstBinding = Binding;
        WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        WriteDescriptorSet.dstArrayElement = ArrayElement;
        WriteDescriptorSet.descriptorCount = static_cast<uint32>(DescriptorImageInfos.size());
        WriteDescriptorSet.pImageInfo = DescriptorImageInfos.data();

        vkUpdateDescriptorSets(Device, 1, &WriteDescriptorSet, 0, nullptr);
    }
}
