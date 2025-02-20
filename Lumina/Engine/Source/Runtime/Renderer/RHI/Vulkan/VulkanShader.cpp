#include "VulkanShader.h"

#include "VulkanCommon.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Source/Runtime/Log/Log.h"
#include "Renderer/RHIIncl.h"
#include "SPIRV-Reflect/spirv_reflect.h"

namespace Lumina
{
	constexpr VkShaderStageFlagBits convert(const EShaderStage& stage)
	{
		switch (stage)
		{
		case EShaderStage::VERTEX:						return VK_SHADER_STAGE_VERTEX_BIT;
		case EShaderStage::FRAGMENT:					return VK_SHADER_STAGE_FRAGMENT_BIT;
		case EShaderStage::COMPUTE:						return VK_SHADER_STAGE_COMPUTE_BIT;
		case EShaderStage::GEOMETRY:					return VK_SHADER_STAGE_GEOMETRY_BIT;
		case EShaderStage::TESSELLATION_CONTROL:		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case EShaderStage::TESSELLATION_EVALUATION:		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		default:										return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}
	}
	
	constexpr VkDescriptorType ToVkDescriptorType(EDescriptorBindingType type)
	{
		switch (type)
		{
			case EDescriptorBindingType::SAMPLER:                    return VK_DESCRIPTOR_TYPE_SAMPLER;
			case EDescriptorBindingType::COMBINED_IMAGE_SAMPLER:     return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case EDescriptorBindingType::SAMPLED_IMAGE:              return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case EDescriptorBindingType::STORAGE_IMAGE:              return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case EDescriptorBindingType::UNIFORM_TEXEL_BUFFER:       return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			case EDescriptorBindingType::STORAGE_TEXEL_BUFFER:       return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			case EDescriptorBindingType::UNIFORM_BUFFER:             return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case EDescriptorBindingType::STORAGE_BUFFER:             return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case EDescriptorBindingType::UNIFORM_BUFFER_DYNAMIC:     return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			case EDescriptorBindingType::STORAGE_BUFFER_DYNAMIC:     return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			case EDescriptorBindingType::INPUT_ATTACHMENT:           return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			case EDescriptorBindingType::ACCELERATION_STRUCTURE_KHR: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			default:                                                 return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	constexpr VkShaderStageFlags ToVkStageFlagBits(uint32 StageFlags)
	{
		VkShaderStageFlags VkFlags = 0;

		if (StageFlags & static_cast<uint32>(EShaderStage::VERTEX))
		{
			VkFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::FRAGMENT))
		{
			VkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::COMPUTE))
		{
			VkFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::GEOMETRY))
		{
			VkFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::TESSELLATION_CONTROL))
		{
			VkFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::TESSELLATION_EVALUATION))
		{
			VkFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::ALL_GRAPHICS))
		{
			VkFlags |= VK_SHADER_STAGE_ALL_GRAPHICS;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::ALL))
		{
			VkFlags |= VK_SHADER_STAGE_ALL;
		}
		if (StageFlags & static_cast<uint32>(EShaderStage::UNKNOWN))
		{
			Assert(0);
		}

		return VkFlags;
	}

	
    FVulkanShader::~FVulkanShader()
    {
		FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

		auto Device = RenderContext->GetDevice();

		for (VkPipelineShaderStageCreateInfo& Stage : VkReflectionData.StageCreateInfos)
		{
			if (Stage.module != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(Device, Stage.module, nullptr);
			}
		}
		
		for (VkDescriptorSetLayout& layout : VkReflectionData.SetLayouts)
		{
			vkDestroyDescriptorSetLayout(Device, layout, nullptr);
		}

		VkReflectionData.StageCreateInfos.clear();
		VkReflectionData.SetLayouts.clear();
    }

    void FVulkanShader::CreateStage(const FShaderStage& StageData)
    {

		FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
		VkDevice Device = RenderContext->GetDevice();

		
		VkShaderModule ShaderModule;

		VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
		ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ShaderModuleCreateInfo.pCode = StageData.Binaries.data();
		ShaderModuleCreateInfo.codeSize = (size_t)StageData.Binaries.size() * 4;
			
		VK_CHECK(vkCreateShaderModule(Device, &ShaderModuleCreateInfo, nullptr, &ShaderModule));

		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo = {};
		ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStageCreateInfo.stage = convert(StageData.Stage);
		ShaderStageCreateInfo.pName = "main";
		ShaderStageCreateInfo.module = ShaderModule;

		VkReflectionData.StageCreateInfos.push_back(ShaderStageCreateInfo);
    }

    void FVulkanShader::GeneratePlatformShaderStageReflectionData(const FShaderReflectionData& ReflectionData)
    {

		FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		 
		VkDevice Device = RenderContext->GetDevice();

		
		
		//---------------------------------------------------------------------------
		// Enumerate Input Bindings
		
		{
			
		}

		
		
		//---------------------------------------------------------------------------
		// Enumerate Descriptor Sets.
		
		{
		    for (const auto& DescriptorBinding : ReflectionData.DescriptorBindings)
		    {
		        TVector<VkDescriptorSetLayoutBinding> Bindings;

		    	TVector<VkDescriptorBindingFlags> BindingFlags;
		
		        for (const auto& KVP : DescriptorBinding)
		        {
		            VkDescriptorSetLayoutBinding LayoutBinding = {};
		            LayoutBinding.binding = KVP.second.Binding;
		            LayoutBinding.descriptorType = ToVkDescriptorType(KVP.second.Type);
		            LayoutBinding.descriptorCount = KVP.second.Count;
		            LayoutBinding.stageFlags = ToVkStageFlagBits(KVP.second.StageFlags);
		
		            VkDescriptorBindingFlags BindingFlag = 0;
		            
		            if (KVP.second.Flags & (uint64)EDescriptorFlags::PARTIALLY_BOUND)
		            {
		                BindingFlag |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		            }
		
		            BindingFlags.push_back(BindingFlag);
		
		            Bindings.push_back(LayoutBinding);
		        }
		
		        VkDescriptorSetLayoutBindingFlagsCreateInfo LayoutBindingsFlags = {};
		        LayoutBindingsFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		        LayoutBindingsFlags.bindingCount = (uint32)BindingFlags.size();
		        LayoutBindingsFlags.pBindingFlags = BindingFlags.data();
		
		        VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
		        LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		        LayoutCreateInfo.pNext = &LayoutBindingsFlags;
		        LayoutCreateInfo.bindingCount = (uint32)Bindings.size();
		        LayoutCreateInfo.pBindings = Bindings.data();
		
		        VkDescriptorSetLayout VkDescriptorSetLayout = VK_NULL_HANDLE;
		        VK_CHECK(vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &VkDescriptorSetLayout));
		
		        VkReflectionData.SetLayouts.push_back(VkDescriptorSetLayout);
		    }
		}


		
		//---------------------------------------------------------------------------
		// Enumerate Push Constant Blocks

		{
			for (const auto& KVP : ReflectionData.PushConstantRanges)
			{
				VkPushConstantRange VulkanPushConstantRange;
				VulkanPushConstantRange.size = KVP.second.Size;
				VulkanPushConstantRange.offset = KVP.second.Offset;
				VulkanPushConstantRange.stageFlags = ToVkStageFlagBits(KVP.second.StageFlags);
				
				VkReflectionData.Ranges.push_back(VulkanPushConstantRange);
			}
		}
    }

    void FVulkanShader::SetFriendlyName(const FString& InString)
    {
	    FShader::SetFriendlyName(InString);
		FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
		
		VkDevice Device = RenderContext->GetDevice();

		VkDebugUtilsObjectNameInfoEXT NameInfo = {};
		NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		NameInfo.pObjectName = GetFriendlyName().c_str();
		NameInfo.objectType = VK_OBJECT_TYPE_SHADER_MODULE;

	    for (auto Stage : VkReflectionData.StageCreateInfos)
	    {
			NameInfo.objectHandle = reinterpret_cast<uint64>(Stage.module);
			RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
	    }

	    for (auto Layout : VkReflectionData.SetLayouts)
	    {
	    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	    	NameInfo.objectHandle = reinterpret_cast<uint64>(Layout);
	    	RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
	    }
		
    }
	
}

