#include "VulkanMaterial.h"

#include "VulkanHelpers.h"
#include "VulkanPipeline.h"
#include "VulkanRenderContext.h"
#include "VulkanSwapChain.h"
#include "Source/Runtime/Renderer/RenderContext.h"

namespace Lumina
{
	
    void GLTFMetallicRoughness::BuildPipelines()
    {

    	FVulkanRenderContext* RenderContext = FRenderContext::Get<FVulkanRenderContext>();
    	VkDevice Device = RenderContext->GetDevice();
    	VkShaderModule FragShader;
		if (FragShader = Vulkan::LoadShaderModule("Resources/Shaders/mesh.frag.spv", Device); FragShader == nullptr)
		{
			LE_LOG_CRITICAL("Failed to load fragment shader");
		}
		
		VkShaderModule VertexShader;
		if (VertexShader = Vulkan::LoadShaderModule("Resources/Shaders/mesh.vert.spv", Device); VertexShader == nullptr)
		{
			LE_LOG_CRITICAL("Failed to load vertex shader");
		}
		
		VkPushConstantRange matrixRange{};
		matrixRange.offset = 0;
		matrixRange.size = sizeof(FGPUDrawPushConstants);
		matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
    	FDescriptorLayoutBuilder layoutBuilder;
    	layoutBuilder.AddBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    	layoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		layoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		
    	MaterialLayout = layoutBuilder.Build(Device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		
		VkDescriptorSetLayout layouts[] = { RenderContext->GetGPUDescriptorLayout(), MaterialLayout };
		
		VkPipelineLayoutCreateInfo mesh_layout_info = Vulkan::PipelineLayoutCreateInfo();
		mesh_layout_info.setLayoutCount = 2;
		mesh_layout_info.pSetLayouts = layouts;
		mesh_layout_info.pPushConstantRanges = &matrixRange;
		mesh_layout_info.pushConstantRangeCount = 1;
		
		VkPipelineLayout newLayout;
		VK_CHECK(vkCreatePipelineLayout(Device, &mesh_layout_info, nullptr, &newLayout));
		
    	OpaquePipeline.Layout = newLayout;
    	TransparentPipeline.Layout = newLayout;
    	
		FVulkanPipeline Pipeline;
		Pipeline.SetShaders(VertexShader, FragShader);
		Pipeline.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		Pipeline.SetPolygonMode(VK_POLYGON_MODE_FILL);
		Pipeline.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		Pipeline.SetMultisampingNone();
		Pipeline.DisableBlending();
		Pipeline.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
		
		//render format
		Pipeline.SetColorAttachmentFormat(RenderContext->GetActiveSwapChain()->GetDrawImage().ImageFormat);
		Pipeline.SetDepthFormat(RenderContext->GetActiveSwapChain()->GetDepthImage().ImageFormat);
		
		// use the triangle layout we created
		Pipeline.PipelineLayout = newLayout;
		
		// finally build the pipeline
    	OpaquePipeline.Pipeline = Pipeline.BuildPipeline(Device);
		
		// create the transparent variant
		Pipeline.EnableBlendingAdditive();
		
		Pipeline.EnableDepthTest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);
		
		TransparentPipeline.Pipeline = Pipeline.BuildPipeline(Device);
		
		vkDestroyShaderModule(Device, FragShader, nullptr);
		vkDestroyShaderModule(Device, VertexShader, nullptr);
    }

    void GLTFMetallicRoughness::ClearResources(VkDevice InDevice)
    {
    }

    FMaterialInstance GLTFMetallicRoughness::WriteMaterial(VkDevice InDevice, EMaterialPass InPass, const Resources& Resources, FDescriptorAllocatorGrowable& Descriptor)
    {
    	FMaterialInstance matData;
    	matData.PassType = InPass;
    	if (InPass == EMaterialPass::Transparent)
    	{
    		matData.Pipeline = &TransparentPipeline;
    	}
    	else
    	{
    		matData.Pipeline = &OpaquePipeline;
    	}

    	matData.MaterialSet = Descriptor.Allocate(InDevice, MaterialLayout);


    	Writer.Clear();
    	Writer.WriteBuffer(0, Resources.DataBuffer, sizeof(Constants), Resources.DataBufferOffset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    	Writer.WriteImage(1, Resources.ColorImage.ImageView, Resources.ColorSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    	Writer.WriteImage(2, Resources.MetalRoughImage.ImageView, Resources.MetalRoughSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    	Writer.UpdateSet(InDevice, matData.MaterialSet);

    	return matData;
    }
}
