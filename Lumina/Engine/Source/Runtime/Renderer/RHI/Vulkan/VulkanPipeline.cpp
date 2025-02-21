#include "VulkanPipeline.h"

#include "VulkanCommon.h"
#include "Containers/Array.h"
#include "VulkanRenderContext.h"
#include "VulkanShader.h"
#include "VulkanImage.h"
#include "Log/Log.h"
#include "VulkanMacros.h"
#include "Renderer/RHIIncl.h"

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

    
    FVulkanPipeline::FVulkanPipeline(const FPipelineSpec& PipelineSpec)
    {
    	Specification = PipelineSpec;	

    	switch (PipelineSpec.PipelineType)
    	{
    		case EPipelineType::GRAPHICS:		this->CreateGraphics(); break;
    		case EPipelineType::COMPUTE:		this->CreateCompute();  break;
    		case EPipelineType::RAY_TRACING:	std::unreachable();
    		default:							std::unreachable();
    	}
    }

    FVulkanPipeline::~FVulkanPipeline()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
    	VkDevice device = RenderContext->GetDevice();
    	vkDestroyPipeline(device, Pipeline, nullptr);
    	vkDestroyPipelineLayout(device, PipelineLayout, nullptr);
    }

    void FVulkanPipeline::SetFriendlyName(const FString& InName)
    {
    	FPipeline::SetFriendlyName(InName);
    		
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	VkDevice Device = RenderContext->GetDevice();
        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = GetFriendlyName().c_str();
    	NameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
    	NameInfo.objectHandle = reinterpret_cast<uint64>(Pipeline);
    	
    	RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);

    	NameInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
    	NameInfo.objectHandle = reinterpret_cast<uint64>(PipelineLayout);
    	
    	RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    void FVulkanPipeline::CreateGraphics()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
    	VkDevice Device = RenderContext->GetDevice();

    	FRHIShader Shader = FShaderLibrary::Get()->GetShader(Specification.GetShader());

    	TRefCountPtr<FVulkanShader> VkShader = Shader.As<FVulkanShader>();
    	const FVulkanShaderReflctionData* VkShaderReflectionData = ((const FVulkanShaderReflctionData*)VkShader->GetPlatformReflectionData());
    		
    	TVector<VkDescriptorSetLayout> DescriptorSetLayouts = VkShaderReflectionData->SetLayouts;
    	TVector<VkPushConstantRange> PushConstantRanges = VkShaderReflectionData->Ranges;

    	TVector<VkVertexInputAttributeDescription> VertexAttributeDescriptions;
	    for (const FPipelineSpec::FVertexAttribute& VertexAttribute : Specification.VertexAttributes)
	    {
	    	VkVertexInputAttributeDescription VkVertexAttribute = {};
	    	VkVertexAttribute.binding = 0;
	    	VkVertexAttribute.format = convert(VertexAttribute.Format);
	    	VkVertexAttribute.location = VertexAttribute.Location;
	    	VkVertexAttribute.offset = VertexAttribute.Offset;
	    	
	    	VertexAttributeDescriptions.push_back(VkVertexAttribute);
	    }

    	
    	
		VkVertexInputBindingDescription VertexBindingDescription = {};
    	VertexBindingDescription.binding = Specification.VertexBinding.Binding;
    	VertexBindingDescription.stride = Specification.VertexBinding.Stride;
    	VertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    	
    	VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};
    	VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    	VertexInputStateInfo.vertexBindingDescriptionCount = VertexAttributeDescriptions.size();
    	VertexInputStateInfo.pVertexBindingDescriptions = &VertexBindingDescription;
    	VertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(VertexAttributeDescriptions.size());
    	VertexInputStateInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();
    	
    	
		/* Input Assembly State */
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {};
		InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyState.topology = convert(Specification.GetPrimitiveTopology());
		InputAssemblyState.primitiveRestartEnable = Specification.GetEnablePrimitiveRestart();

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

    	/* Conservative rasterization state */
    	VkPipelineRasterizationConservativeStateCreateInfoEXT ConservativeRasterInfo = {};
    	ConservativeRasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
    	ConservativeRasterInfo.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;
    	ConservativeRasterInfo.extraPrimitiveOverestimationSize = 0.0f;
    		
    	/* Rasterization State */	
		VkPipelineRasterizationStateCreateInfo RasterizationState = {};
		RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizationState.depthClampEnable = VK_FALSE;
		RasterizationState.rasterizerDiscardEnable = VK_FALSE;
		RasterizationState.polygonMode = Specification.GetPolygonFillMode() == EPipelineFillMode::FILL ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
		RasterizationState.lineWidth = Specification.GetLineWidth();
		RasterizationState.cullMode = convert(Specification.GetCullingMode());
		RasterizationState.frontFace = convert(Specification.GetFaceOrientation());
    	RasterizationState.pNext = Specification.GetUseConservativeRasterization() ? &ConservativeRasterInfo : nullptr;
    		
    	/* Viewport */	
		VkPipelineViewportStateCreateInfo ViewportState = {};
		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.scissorCount = 1;

    	/* Blend States */	
		TVector<VkPipelineColorBlendAttachmentState> BlendStates(Specification.GetRenderTargetFormats().size());
		for (VkPipelineColorBlendAttachmentState& state : BlendStates)
		{
			state.blendEnable = Specification.GetEnableAlphaBlending();
			state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			state.colorBlendOp = VK_BLEND_OP_ADD;
			state.srcAlphaBlendFactor = static_cast<VkBlendFactor>(Specification.GetAlphaBlendSrcFactor());
			state.dstAlphaBlendFactor = static_cast<VkBlendFactor>(Specification.GetAlphaBlendDstFactor());
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
		DepthStencilState.depthTestEnable = Specification.GetEnableDepthTest();
		DepthStencilState.depthWriteEnable = Specification.GetEnableDepthTest();
		DepthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.stencilTestEnable = VK_FALSE;

    	/* MSAA */	
		VkPipelineMultisampleStateCreateInfo MultisampleState = {};
		MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultisampleState.sampleShadingEnable = Specification.GetEnableMultisampling();
		MultisampleState.rasterizationSamples = (VkSampleCountFlagBits)Specification.GetSampleCount();
    	
    	/* Pipeline Layout */	
		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
		PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutCreateInfo.setLayoutCount = (uint32)DescriptorSetLayouts.size();
		PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
		PipelineLayoutCreateInfo.pushConstantRangeCount = (uint32)PushConstantRanges.size();
		PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();

		VK_CHECK(vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, nullptr, &PipelineLayout));

		TVector<VkFormat> formats;
    	formats.reserve(Specification.GetRenderTargetFormats().size());
		for (const EImageFormat& format : Specification.GetRenderTargetFormats())
		{
			formats.emplace_back(convert(format));
		}

    	/* Pipeline Rendering Info */	
		VkPipelineRenderingCreateInfo PipelineRendering = {};
		PipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		PipelineRendering.colorAttachmentCount = (uint32)formats.size();
		PipelineRendering.pColorAttachmentFormats = formats.data();
		// HACK: I assume that D32_SFLOAT is chosen format. More proper way to do it is to request depth image format from pipeline spec or swapchain
		PipelineRendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;


		TVector<VkPipelineShaderStageCreateInfo> StageInfos = VkShaderReflectionData->StageCreateInfos;

    	/* Graphics Pipeline Creation */	
		VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
    	ZeroVkStruct(GraphicsPipelineCreateInfo, VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);
		GraphicsPipelineCreateInfo.pNext = &PipelineRendering;
		GraphicsPipelineCreateInfo.pStages = StageInfos.data();
		GraphicsPipelineCreateInfo.stageCount = (uint32)StageInfos.size();
		GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateInfo;
		GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
		GraphicsPipelineCreateInfo.pDynamicState = &DynamicState;
		GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationState;
		GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendState;
		GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilState;
		GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleState;
		GraphicsPipelineCreateInfo.layout = PipelineLayout;
		GraphicsPipelineCreateInfo.pViewportState = &ViewportState;
		GraphicsPipelineCreateInfo.renderPass = nullptr; // Dynamic rendering

		VK_CHECK(vkCreateGraphicsPipelines(Device, nullptr, 1, &GraphicsPipelineCreateInfo, nullptr, &Pipeline));

    }

    void FVulkanPipeline::CreateCompute()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
    	VkDevice Device = RenderContext->GetDevice();

    	const FVulkanShaderReflctionData* VkShaderReflectionData;//TODO = ((const FVulkanShaderReflctionData*)Specification.GetShader()->GetPlatformReflectionData());
	
    	TVector<VkDescriptorSetLayout> DescriptorSetLayouts = VkShaderReflectionData->SetLayouts;
    	TVector<VkPushConstantRange> PushConstantRanges = VkShaderReflectionData->Ranges;

    	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    	PipelineLayoutCreateInfo.setLayoutCount = (uint32)DescriptorSetLayouts.size();
    	PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
    	PipelineLayoutCreateInfo.pushConstantRangeCount = (uint32)PushConstantRanges.size();
    	PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();

    	VK_CHECK(vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, nullptr, &PipelineLayout));

    	TVector<VkPipelineShaderStageCreateInfo> StageCreateInfo = VkShaderReflectionData->StageCreateInfos;

    	VkComputePipelineCreateInfo ComputePipelineCreateInfo = {};
    	ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    	ComputePipelineCreateInfo.layout = PipelineLayout;
    	ComputePipelineCreateInfo.stage = StageCreateInfo[0];

    	VK_CHECK(vkCreateComputePipelines(Device, nullptr, 1, &ComputePipelineCreateInfo, nullptr, &Pipeline));

    }
}
