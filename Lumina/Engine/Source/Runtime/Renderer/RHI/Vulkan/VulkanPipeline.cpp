#include "VulkanPipeline.h"

#include "Containers/Array.h"
#include "VulkanRenderContext.h"
#include "VulkanShader.h"
#include "VulkanImage.h"
#include "VulkanSwapchain.h"
#include "Log/Log.h"
#include "VulkanMacros.h"

namespace Lumina
{


    	constexpr VkFormat convert(const EShaderDataType& type)
		{
		switch (type)
		{
			case EShaderDataType::INT:				return VK_FORMAT_R32_SINT;
			case EShaderDataType::INT2:				return VK_FORMAT_R32G32_SINT;
			case EShaderDataType::INT3:				return VK_FORMAT_R32G32B32_SINT;
			case EShaderDataType::INT4:				return VK_FORMAT_R32G32B32A32_SINT;
			case EShaderDataType::FLOAT:			return VK_FORMAT_R32_SFLOAT;
			case EShaderDataType::FLOAT2:			return VK_FORMAT_R32G32_SFLOAT;
			case EShaderDataType::FLOAT3:			return VK_FORMAT_R32G32B32_SFLOAT;
			case EShaderDataType::FLOAT4:			return VK_FORMAT_R32G32B32A32_SFLOAT;
			case EShaderDataType::IMAT3:			return VK_FORMAT_R32G32B32_SINT;
			case EShaderDataType::IMAT4:			return VK_FORMAT_R32G32B32A32_SINT;
			case EShaderDataType::MAT3:				return VK_FORMAT_R32G32B32_SFLOAT;
			case EShaderDataType::MAT4:				return VK_FORMAT_R32G32B32A32_SFLOAT;
			default:								std::unreachable();
		}
	}

	constexpr VkPrimitiveTopology convert(const EPipelineTopology& topology)
	{
		switch (topology)
		{
			case EPipelineTopology::TRIANGLES:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case EPipelineTopology::LINES:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case EPipelineTopology::POINTS:			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			default:								std::unreachable();
		}
	}

	constexpr VkPolygonMode convert(const EPipelineFillMode& mode)
	{
		switch (mode)
		{
			case EPipelineFillMode::FILL:			return VK_POLYGON_MODE_FILL;
			case EPipelineFillMode::EDGE_ONLY:		return VK_POLYGON_MODE_LINE;
			default:								std::unreachable();
		}
	}

	constexpr VkCullModeFlagBits convert(const EPipelineCullingMode& mode)
	{
		switch (mode)
		{
			case EPipelineCullingMode::BACK:		return VK_CULL_MODE_BACK_BIT;
			case EPipelineCullingMode::FRONT:		return VK_CULL_MODE_FRONT_BIT;
			case EPipelineCullingMode::NONE:		return VK_CULL_MODE_NONE;
			default:								std::unreachable();
		}
	}

	constexpr VkFrontFace convert(const EPipelineFrontFace face)
	{
		switch (face)
		{
			case EPipelineFrontFace::CLOCKWISE:				return VK_FRONT_FACE_CLOCKWISE;
			case EPipelineFrontFace::COUNTER_CLOCKWISE:		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			default:										std::unreachable();
		}
	}

    
    FVulkanPipeline::FVulkanPipeline(const FPipelineSpecification& InSpec)
    {
    	Specification = InSpec;	
        Guid = FGuid::Generate();

    	switch (InSpec.type)
    	{
    		case EPipelineType::GRAPHICS:		CreateGraphics(); break;
    		case EPipelineType::COMPUTE:		CreateCompute();  break;
    		case EPipelineType::RAY_TRACING:	std::unreachable();
    		default:							std::unreachable();
    	}
    }

    FVulkanPipeline::~FVulkanPipeline()
    {

    }

    void FVulkanPipeline::Destroy()
    {
    	auto device = FVulkanRenderContext::GetDevice();
    	vkDestroyPipeline(device, Pipeline, nullptr);
    	vkDestroyPipelineLayout(device, PipelineLayout, nullptr);
    }

    void FVulkanPipeline::CreateGraphics()
    {
		auto Device = FVulkanRenderContext::GetDevice();

    	/* Vertex Input Binding */	
		VkVertexInputBindingDescription VertexInputBinding = {};
		VertexInputBinding.binding = 0;
		VertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		VertexInputBinding.stride = Specification.input_layout.GetStride();
		std::vector<VkVertexInputAttributeDescription> VertexInputAttributes;

		VkPipelineVertexInputStateCreateInfo VertexInputState = {};
		VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		if (Specification.input_layout.GetStride())
		{
			int PreviousLocWidth = 0;
			for (const auto& element : Specification.input_layout.GetElements())
			{
				uint32 location;
				float LocationComputeResult = (float)element.Size / 16.0f;
				if (LocationComputeResult <= 1.0f)
				{
					location = PreviousLocWidth;
					PreviousLocWidth += 1;
				}
				else
				{
					location = PreviousLocWidth;
					PreviousLocWidth += (int32)LocationComputeResult;
				}

				VertexInputAttributes.push_back(
				{
					.location = location,
					.binding = 0,
					.format = convert(element.Format),
					.offset = element.Offset
				});
			}

			VertexInputState.vertexBindingDescriptionCount = 1;
			VertexInputState.pVertexBindingDescriptions = &VertexInputBinding;
			VertexInputState.pVertexAttributeDescriptions = VertexInputAttributes.data();
			VertexInputState.vertexAttributeDescriptionCount = (uint32)VertexInputAttributes.size();
		}
		else
		{
			VertexInputState.vertexBindingDescriptionCount = 0;
			VertexInputState.pVertexBindingDescriptions = nullptr;
			VertexInputState.vertexAttributeDescriptionCount = 0;
			VertexInputState.pVertexAttributeDescriptions = nullptr;
		}
		
		/* Input Assembly State */
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {};
		InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyState.topology = convert(Specification.topology);
		InputAssemblyState.primitiveRestartEnable = Specification.primitive_restart_enable;

		VkDynamicState DynamicStates[] =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

    	/* Dynamic States */	
		VkPipelineDynamicStateCreateInfo DynamicState = {};
		DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicState.pDynamicStates = DynamicStates;
		DynamicState.dynamicStateCount = 2;

    	/* Rasterization State */	
		VkPipelineRasterizationStateCreateInfo RasterizationState = {};
		RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizationState.depthClampEnable = VK_FALSE;
		RasterizationState.rasterizerDiscardEnable = VK_FALSE;
		RasterizationState.polygonMode = Specification.fill_mode == EPipelineFillMode::FILL ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
		RasterizationState.lineWidth = Specification.line_width;
		RasterizationState.cullMode = convert(Specification.culling_mode);
		RasterizationState.frontFace = convert(Specification.front_face);
    		
    	/* Viewport */	
		VkPipelineViewportStateCreateInfo ViewportState = {};
		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.scissorCount = 1;

    	/* Blend States */	
		std::vector<VkPipelineColorBlendAttachmentState> BlendStates(Specification.output_attachments_formats.size());
		for (auto& state : BlendStates)
		{
			state.blendEnable = Specification.color_blending_enable;
			state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			state.colorBlendOp = VK_BLEND_OP_ADD;
			state.srcAlphaBlendFactor = static_cast<VkBlendFactor>(Specification.SrcAlphaBlendFactor);//VK_BLEND_FACTOR_ZERO;
			state.dstAlphaBlendFactor = static_cast<VkBlendFactor>(Specification.DstAlphaBlendFactor);//VK_BLEND_FACTOR_ONE;
			state.alphaBlendOp = VK_BLEND_OP_ADD;
			state.colorWriteMask = VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
		}

    	/* Color Blend State */	
		VkPipelineColorBlendStateCreateInfo ColorBlendState = {};
		ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlendState.attachmentCount = (uint32)BlendStates.size();
		ColorBlendState.pAttachments = BlendStates.data();
		ColorBlendState.logicOpEnable = VK_FALSE;
		ColorBlendState.logicOp = VK_LOGIC_OP_COPY;

		/* Depth Stencil State */
		VkPipelineDepthStencilStateCreateInfo DepthStencilState = {};
		DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencilState.depthTestEnable = Specification.depth_test_enable;
		DepthStencilState.depthWriteEnable = Specification.depth_test_enable;
		DepthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.stencilTestEnable = VK_FALSE;

    	/* MSAA */	
		VkPipelineMultisampleStateCreateInfo MultisampleState = {};
		MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultisampleState.sampleShadingEnable = Specification.multisampling_enable;
		MultisampleState.rasterizationSamples = (VkSampleCountFlagBits)Specification.sample_count;

		TRefPtr<FVulkanShader> VkShader = RefPtrCast<FVulkanShader>(Specification.shader);
		TVector<VkDescriptorSetLayout> DescriptorSetLayouts = VkShader->GetLayouts();
		TVector<VkPushConstantRange> PushConstantRanges = VkShader->GetRanges();

    	/* Pipeline Layout */	
		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
		PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutCreateInfo.setLayoutCount = (uint32)DescriptorSetLayouts.size();
		PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
		PipelineLayoutCreateInfo.pushConstantRangeCount = (uint32)PushConstantRanges.size();
		PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();

		vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, nullptr, &PipelineLayout);

		std::vector<VkFormat> formats;
		for (const auto& format : Specification.output_attachments_formats)
		{
			formats.push_back(convert(format));
		}

    	/* Pipeline Rendering Info */	
		VkPipelineRenderingCreateInfo PipelineRendering = {};
		PipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		PipelineRendering.colorAttachmentCount = (uint32)formats.size();
		PipelineRendering.pColorAttachmentFormats = formats.data();
		// HACK: I assume that D32_SFLOAT is chosen format. More proper way to do it is to request depth image format from pipeline spec or swapchain
		PipelineRendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;


		TVector<VkPipelineShaderStageCreateInfo> StageInfos = VkShader->GetCreateInfos();

    	/* Graphics Pipeline Creation */	
		VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
		GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		GraphicsPipelineCreateInfo.pNext = &PipelineRendering;
		GraphicsPipelineCreateInfo.pStages = StageInfos.data();
		GraphicsPipelineCreateInfo.stageCount = (uint32)StageInfos.size();
		GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputState;
		GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
		GraphicsPipelineCreateInfo.pDynamicState = &DynamicState;
		GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationState;
		GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendState;
		GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilState;
		GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleState;
		GraphicsPipelineCreateInfo.layout = PipelineLayout;
		GraphicsPipelineCreateInfo.pViewportState = &ViewportState;
		GraphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;

		VK_CHECK(vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &Pipeline));

		LOG_TRACE("Pipeline {0} created successfully", Specification.DebugName);	
    }

    void FVulkanPipeline::CreateCompute()
    {

    	auto Device = FVulkanRenderContext::GetDevice();	
	
    	TRefPtr<FVulkanShader> VkShader = RefPtrCast<FVulkanShader>(Specification.shader);
    	TVector<VkDescriptorSetLayout> DescriptorSetLayouts = VkShader->GetLayouts();
    	TVector<VkPushConstantRange> PushConstantRanges = VkShader->GetRanges();

    	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    	PipelineLayoutCreateInfo.setLayoutCount = (uint32)DescriptorSetLayouts.size();
    	PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
    	PipelineLayoutCreateInfo.pushConstantRangeCount = (uint32)PushConstantRanges.size();
    	PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();

    	vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, nullptr, &PipelineLayout);

    	TVector<VkPipelineShaderStageCreateInfo> StageCreateInfo = VkShader->GetCreateInfos();

    	VkComputePipelineCreateInfo ComputePipelineCreateInfo = {};
    	ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    	ComputePipelineCreateInfo.layout = PipelineLayout;
    	ComputePipelineCreateInfo.stage = StageCreateInfo[0];

    	VK_CHECK(vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1, &ComputePipelineCreateInfo, nullptr, &Pipeline));

    	LOG_TRACE("Pipeline {0} created successfully", Specification.DebugName);	
    }
}
