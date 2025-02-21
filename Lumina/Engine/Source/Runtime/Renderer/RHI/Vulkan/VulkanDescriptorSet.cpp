#include "VulkanDescriptorSet.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanRenderAPI.h"
#include "VulkanRenderContext.h"
#include "Renderer/RHIIncl.h"
#include "VulkanMacros.h"

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

    const VkShaderStageFlags GetShaderStageFlags(uint32 stage)
    {
        VkShaderStageFlags flags = 0;

        if (stage & (uint32)EShaderStage::VERTEX)
        {
            flags |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        if (stage & (uint32)EShaderStage::FRAGMENT)
        {
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if (stage & (uint32)EShaderStage::COMPUTE)
        {
            flags |= VK_SHADER_STAGE_COMPUTE_BIT;
        }
        if (stage & (uint32)EShaderStage::GEOMETRY)
        {
            flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        }
        if (stage & (uint32)EShaderStage::TESSELLATION_CONTROL)
        {
            flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        }
        if (stage & (uint32)EShaderStage::TESSELLATION_EVALUATION)
        {
            flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }
        if (stage & (uint32)EShaderStage::ALL_GRAPHICS)
        {
            flags |= VK_SHADER_STAGE_ALL_GRAPHICS;
        }
        if (stage & (uint32)EShaderStage::ALL)
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
        : DescriptorSet(VK_NULL_HANDLE)
        , Layout(VK_NULL_HANDLE)
    {
        
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

        auto Device = RenderContext->GetDevice();

        TVector<VkDescriptorSetLayoutBinding> SetBindings;
        TVector<VkDescriptorBindingFlags> BindingFlags;

        for (const FDescriptorBinding& binding : InSpec.Bindings)
        {
            VkDescriptorSetLayoutBinding VkBinding = {};
            VkBinding.stageFlags = GetShaderStageFlags(binding.StageFlags);
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

        VK_CHECK(vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &Layout));

        TVector<VkDescriptorSet> set = FRenderer::GetRenderContext<FVulkanRenderContext>()->AllocateDescriptorSets(Layout, 1);
        DescriptorSet = set[0];
    }

    FVulkanDescriptorSet::~FVulkanDescriptorSet()
    {
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = RenderContext->GetDevice();
        
        AssertMsg(Device != VK_NULL_HANDLE, "Device was null when trying to destroy a descriptor set!");
        if (Layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(Device, Layout, nullptr);
        }
        
        FRenderer::GetRenderContext<FVulkanRenderContext>()->FreeDescriptorSets({ DescriptorSet });
    }

    void FVulkanDescriptorSet::Write(uint16 Binding, uint16 ArrayElement, FRHIBuffer Buffer, uint64 Size, uint64 Offset)
    {
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = RenderContext->GetDevice();
        
        TRefCountPtr<FVulkanBuffer> vkBuffer = Buffer.As<FVulkanBuffer>();

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

    void FVulkanDescriptorSet::Write(uint16 Binding, uint16 ArrayElement, FRHIImage Image)
    {
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = RenderContext->GetDevice();
        
        TRefCountPtr<FVulkanImage> vkImage = Image.As<FVulkanImage>();
        TRefCountPtr<FVulkanImageSampler> vk_sampler = FRenderer::GetLinearSampler().As<FVulkanImageSampler>();

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

    void FVulkanDescriptorSet::Write(uint16 Binding, uint16 ArrayElement, TVector<FRHIImage> Images)
    {
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = RenderContext->GetDevice();
    
        TVector<VkDescriptorImageInfo> DescriptorImageInfos(Images.size());

        for (size_t i = 0; i < Images.size(); ++i)
        {
            TRefCountPtr<FVulkanImage> vkImage = Images[i].As<FVulkanImage>();
            TRefCountPtr<FVulkanImageSampler> vkSampler = FRenderer::GetLinearSampler().As<FVulkanImageSampler>();

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

    void FVulkanDescriptorSet::SetFriendlyName(const FString& InName)
    {
        FDescriptorSet::SetFriendlyName(InName);

        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = RenderContext->GetDevice();

        VkDebugUtilsObjectNameInfoEXT NameInfo = {};
        NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        NameInfo.pObjectName = GetFriendlyName().c_str();
        NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
        NameInfo.objectHandle = reinterpret_cast<uint64_t>(DescriptorSet);
        
        RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);

        NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
        NameInfo.objectHandle = reinterpret_cast<uint64_t>(Layout);

        RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);

    }
}
