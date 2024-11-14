#include "VulkanShader.h"

#include "VulkanRenderContext.h"
#include "Source/Runtime/Log/Log.h"

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
	std::string ShaderStageFlagsToString(VkShaderStageFlags flags)
	{
		std::ostringstream result;

		if (flags & VK_SHADER_STAGE_VERTEX_BIT)
			result << "VERTEX ";
		if (flags & VK_SHADER_STAGE_FRAGMENT_BIT)
			result << "FRAGMENT ";
		if (flags & VK_SHADER_STAGE_COMPUTE_BIT)
			result << "COMPUTE ";
		if (flags & VK_SHADER_STAGE_GEOMETRY_BIT)
			result << "GEOMETRY ";
		if (flags & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
			result << "TESSELLATION_CONTROL ";
		if (flags & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
			result << "TESSELLATION_EVALUATION ";
		if (flags & VK_SHADER_STAGE_RAYGEN_BIT_KHR)
			result << "RAYGEN ";
		if (flags & VK_SHADER_STAGE_ANY_HIT_BIT_KHR)
			result << "ANY_HIT ";
		if (flags & VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
			result << "CLOSEST_HIT ";
		if (flags & VK_SHADER_STAGE_MISS_BIT_KHR)
			result << "MISS ";
		if (flags & VK_SHADER_STAGE_INTERSECTION_BIT_KHR)
			result << "INTERSECTION ";
		if (flags & VK_SHADER_STAGE_CALLABLE_BIT_KHR)
			result << "CALLABLE ";

		// If no flags were set, indicate an "UNKNOWN" stage
		if (result.str().empty())
			result << "UNKNOWN";

		return result.str();
	}

	
	constexpr VkDescriptorType convert(SpvReflectDescriptorType Type)
	{
		switch (Type)
		{
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:							return VK_DESCRIPTOR_TYPE_SAMPLER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:						return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:						return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:				return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:				return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:					return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			default:															std::unreachable();
		}
	}

	constexpr std::string DescriptorToString(VkDescriptorType type)
	{
		switch (type)
		{
			case VK_DESCRIPTOR_TYPE_SAMPLER:						return "sampler";
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:			return "combined image sampler";
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:					return "sampled image";
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:					return "storage image";
			case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:			return "uniform texel buffer";
			case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:			return "storage texel buffer";
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:					return "uniform buffer";
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:			return "uniform buffer dynamic";
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:					return "storage buffer";
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:			return "storage buffer dynamic";
			default:												std::unreachable();
		}
	}
	
    FVulkanShader::FVulkanShader(const TFastVector<FShaderData>& InData, const LString& Tag)
	{
		auto Device = FVulkanRenderContext::GetDevice();

		std::map<uint32, TFastVector<VkDescriptorSetLayoutBinding>> Bindings;

		for (auto& StageData : InData)
		{
			VkPushConstantRange PushConstantRange = {};
			VkShaderModule ShaderModule;

			VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
			ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			ShaderModuleCreateInfo.pCode = StageData.Binaries.data();
			ShaderModuleCreateInfo.codeSize = (size_t)StageData.Binaries.size() * 4;
			
			vkCreateShaderModule(Device, &ShaderModuleCreateInfo, nullptr, &ShaderModule);

			VkPipelineShaderStageCreateInfo ShaderStageCreateInfo = {};
			ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ShaderStageCreateInfo.stage = convert(StageData.Stage);
			ShaderStageCreateInfo.pName = "main";
			ShaderStageCreateInfo.module = ShaderModule;

			StageCreateInfos.push_back(ShaderStageCreateInfo);

			SpvReflectShaderModule ReflectModule;
			if (spvReflectCreateShaderModule((size_t)StageData.Binaries.size() * 4, StageData.Binaries.data(), &ReflectModule) != SPV_REFLECT_RESULT_SUCCESS)
			{
				LOG_ERROR("Failed to reflect shader {0}", Path.filename().string());
				bDirty = true;
				return;
			}

			uint32 set_count = 0;
			spvReflectEnumerateDescriptorSets(&ReflectModule, &set_count, nullptr);

			TFastVector<SpvReflectDescriptorSet*> ReflectDescriptorSets(set_count);
			spvReflectEnumerateDescriptorSets(&ReflectModule, &set_count, ReflectDescriptorSets.data());

			for (SpvReflectDescriptorSet* ReflectSet : ReflectDescriptorSets)
			{
				if (!Bindings.contains(ReflectSet->set))
				{
					Bindings.emplace(ReflectSet->set, TFastVector<VkDescriptorSetLayoutBinding>());
				}

				for (int i = 0; i < ReflectSet->binding_count; i++)
				{
					SpvReflectDescriptorBinding* ReflectBinding = ReflectSet->bindings[i];

					VkDescriptorSetLayoutBinding layout_binding = {};
					layout_binding.binding = ReflectBinding->binding;
					layout_binding.descriptorType = convert(ReflectBinding->descriptor_type);
					layout_binding.descriptorCount = ReflectBinding->count;
					//@TODO Figure out how to properly specify the layout stage... changing to anything but STAGE_ALL will cause validation erros.
					layout_binding.stageFlags = StageData.Stage == EShaderStage::COMPUTE ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL;

					bool bSkipBinding = false;

					for (auto& set_binding : Bindings[ReflectSet->set])
					{
						if (set_binding.binding == layout_binding.binding)
						{
							bSkipBinding = true;
							break;
						}
					}
					
					if(!bSkipBinding)
					{
						Bindings[ReflectSet->set].push_back(layout_binding);
					}
				}
			}

			uint32 PushConstantRangeCount = 0;
			spvReflectEnumeratePushConstantBlocks(&ReflectModule, &PushConstantRangeCount, nullptr);

			std::vector<SpvReflectBlockVariable*> ReflectPushConstantRanges(PushConstantRangeCount);
			spvReflectEnumeratePushConstantBlocks(&ReflectModule, &PushConstantRangeCount, ReflectPushConstantRanges.data());

			if (PushConstantRangeCount)
			{
				for (int i = 0; i < ReflectPushConstantRanges[0]->member_count; i++)
				{
					auto& member = ReflectPushConstantRanges[0]->members[i];
				}
			}

			for (SpvReflectBlockVariable* ReflectRange : ReflectPushConstantRanges)
			{
				PushConstantRange.size = ReflectRange->size;
				PushConstantRange.offset = ReflectRange->offset;
				PushConstantRange.stageFlags = convert(StageData.Stage);
				Ranges.push_back(PushConstantRange);
			}

			spvReflectDestroyShaderModule(&ReflectModule);

		}


		LOG_TRACE("===============================");
		LOG_TRACE("Reflecting Shader - {0}", Tag);
		
		for (auto& set : Bindings)
		{
			LOG_TRACE("Set #{0}: ", set.first);
			for (auto& binding : set.second)
			{
				LOG_TRACE("Setting up bindings: #{0}: {1}[{2}]", binding.binding, DescriptorToString(binding.descriptorType), binding.descriptorCount);
			}
		}

		for(auto& Range : Ranges)
		{
			LOG_TRACE("Setting up push constants: Stage: {0}, Offset: {1}, Size: {2}", Range.stageFlags, Range.offset, Range.size);
		}

		LOG_TRACE("===============================");


		if(Bindings.size())
		{
			uint32 highest_set_index = Bindings.rbegin()->first;
			for (uint32 i = 0; i <= highest_set_index; i++)
			{
				if (Bindings.find(i) == Bindings.end())
				{
					SetLayouts.push_back(VK_NULL_HANDLE);
				}
				else
				{
					std::vector<VkDescriptorBindingFlags> BindingFlags;
					
					for (auto& binding : Bindings[i])
					{
						if (binding.descriptorCount != 1) BindingFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
						else BindingFlags.push_back(0);
					}

					VkDescriptorSetLayoutBindingFlagsCreateInfo LayoutBindingsFlags = {};
					LayoutBindingsFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
					LayoutBindingsFlags.bindingCount = (uint32)BindingFlags.size();
					LayoutBindingsFlags.pBindingFlags = BindingFlags.data();

					VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
					LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					LayoutCreateInfo.pNext = &LayoutBindingsFlags;
					LayoutCreateInfo.bindingCount = (uint32)Bindings[i].size();
					LayoutCreateInfo.pBindings = Bindings[i].data();

					VkDescriptorSetLayout VkDescriptorSetLayout = VK_NULL_HANDLE;
					vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &VkDescriptorSetLayout);
					SetLayouts.push_back(VkDescriptorSetLayout);
				}
			}
		}
	}
	
    FVulkanShader::~FVulkanShader()
    {
    }

    void FVulkanShader::RestoreShaderModule(std::filesystem::path path)
    {

    }

    void FVulkanShader::Destroy()
    {
		auto Device = FVulkanRenderContext::GetDevice();

		for (auto& Stage : StageCreateInfos)
		{
			if (Stage.module != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(Device, Stage.module, nullptr);
			}
		}
		
		for (auto& layout : SetLayouts)
		{
			vkDestroyDescriptorSetLayout(Device, layout, nullptr);
		}

		SetLayouts.clear();
		
		for (auto& stage : StageCreateInfos)
		{
			stage.module = VK_NULL_HANDLE;
		}
    }
}
