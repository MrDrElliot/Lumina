#include "VulkanImGuiRender.h"

#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "Renderer/Pipeline.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/Application/Application.h"
#include "Core/Windows/Window.h"
#include "Renderer/Shader.h"
#include "Renderer/RHIIncl.h"

#include "Renderer/RHI/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/RHI/Vulkan/VulkanImage.h"
#include "Renderer/RHI/Vulkan/VulkanMacros.h"
#include "Renderer/RHI/Vulkan/VulkanRenderContext.h"
#include "Renderer/RHI/Vulkan/VulkanSwapchain.h"
#include "Scene/Scene.h"

namespace Lumina
{
	FVulkanImGuiRender::FVulkanImGuiRender()
	{
	}

    void FVulkanImGuiRender::Initialize(const FSubsystemManager& Manager)
    {
		IImGuiRenderer::Initialize(Manager);
		
	     LOG_TRACE("Vulkan ImGui Renderer: Init");
        
        VkDescriptorPoolSize PoolSizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

        VkDescriptorPoolCreateInfo PoolInfo =  {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        PoolInfo.maxSets = 1000;
        PoolInfo.poolSizeCount = (uint32_t)std::size(PoolSizes);
        PoolInfo.pPoolSizes = PoolSizes;

    	FVulkanRenderContext* VkRenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	VkDevice Device = VkRenderContext->GetDevice();
    	
        VK_CHECK(vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &DescriptorPool));

        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = "ImGui Descriptor Pool";
    	NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
    	NameInfo.objectHandle = reinterpret_cast<uint64_t>(DescriptorPool);
    	
    	VkRenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
		
		
        Assert(ImGui_ImplGlfw_InitForVulkan(FApplication::Get().GetMainWindow()->GetWindow(), true));

        VkPipelineRenderingCreateInfo RenderPipeline = {};
        RenderPipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        RenderPipeline.pColorAttachmentFormats = &VkRenderContext->GetSwapchain<FVulkanSwapchain>()->GetFormat();
        RenderPipeline.colorAttachmentCount = 1;

    	
        ImGui_ImplVulkan_InitInfo InitInfo = {};
        InitInfo.PipelineRenderingCreateInfo = RenderPipeline;
        InitInfo.Instance = VkRenderContext->GetVulkanInstance();
        InitInfo.PhysicalDevice = VkRenderContext->GetPhysicalDevice();
        InitInfo.Device = VkRenderContext->GetDevice();
        InitInfo.Queue = VkRenderContext->GetGeneralQueue();
        InitInfo.DescriptorPool = DescriptorPool;
        InitInfo.MinImageCount = 2;
        InitInfo.ImageCount = 2;
        InitInfo.UseDynamicRendering = true;
        InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;


        Assert(ImGui_ImplVulkan_Init(&InitInfo));
        Assert(ImGui_ImplVulkan_CreateFontsTexture());

		
    }

    void FVulkanImGuiRender::Deinitialize()
    {
		FRenderer::WaitIdle();
    	for (auto& KVP : ImageCache)
    	{
    		ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(KVP.second.ID));
    	}
    	
    	ImGui_ImplVulkan_Shutdown();
    	
    	FVulkanRenderContext* VkRenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	VkDevice Device = VkRenderContext->GetDevice();
		
    	vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
    	
    	ImGui_ImplGlfw_Shutdown();
    	ImGui::DestroyContext();
    }

    void FVulkanImGuiRender::OnStartFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
	
	void FVulkanImGuiRender::RenderImage(FRHIImage Image, ImVec2 Size)
    {
    	FGuid Guid = Image->GetGuid();
	    if (ImageCache.find(Guid) == ImageCache.end())
	    {
	    	TRefCountPtr<FVulkanImageSampler> Sampler = FRenderer::GetLinearSampler().As<FVulkanImageSampler>();
    	
	    	VkDescriptorSet ImGuiImageID = ImGui_ImplVulkan_AddTexture(Sampler->GetSampler(),
	    			Image.As<FVulkanImage>()->GetImageView(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	    	ImGuiX::FImGuiImageInfo NewInfo;
            NewInfo.Image = Image;
	    	NewInfo.Size = Size;
	    	NewInfo.ID = reinterpret_cast<ImTextureID>(ImGuiImageID);
	    	ImageCache.emplace(Guid, NewInfo);

	    	RegisterNewImage(Guid, NewInfo.ID, Image->GetFriendlyName());
	    	RenderImage(NewInfo);

	    }
	    else
	    {
		    RenderImage(ImageCache[Guid]);
	    }
    }
	
    void FVulkanImGuiRender::RenderImage(const ImGuiX::FImGuiImageInfo& ImageInfo)
    {
        ImGui::Image(ImageInfo.ID, ImageInfo.Size, { 0, 0 }, { 1, 1 });
    }

	ImGuiX::FImGuiImageInfo FVulkanImGuiRender::CreateImGuiTexture(FRHIImage Image, ImVec2 Size)
	{
		auto It = ImageCache.find(Image->GetGuid());
		if (It != ImageCache.end())
		{
			return It->second;
		}

		TRefCountPtr<FVulkanImage> VkImage = Image.As<FVulkanImage>();
		TRefCountPtr<FVulkanImageSampler> Sampler = FRenderer::GetLinearSampler().As<FVulkanImageSampler>();
    
		VkDescriptorSet ImGuiImageID = ImGui_ImplVulkan_AddTexture(Sampler->GetSampler(), VkImage->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		ImGuiX::FImGuiImageInfo NewInfo;
        NewInfo.Image = Image;
		NewInfo.Size = Size;
		NewInfo.ID = reinterpret_cast<ImTextureID>(ImGuiImageID);
		ImageCache.emplace(Image->GetGuid(), NewInfo);

		RegisterNewImage(Image->GetGuid(), NewInfo.ID, "ImGui_" + Image->GetFriendlyName());
    
		return NewInfo;
	}

	ImGuiX::FImGuiImageInfo FVulkanImGuiRender::CreateImGuiTexture(const FString& RawPath)
	{
		TRefCountPtr<FVulkanImage> Image = FTextureFactory::ImportFromSource(RawPath.c_str()).As<FVulkanImage>();
		Image->SetFriendlyName(RawPath);
    
		auto It = ImageCache.find(Image->GetGuid());
		if (It != ImageCache.end())
		{
			return It->second;
		}
    
		TRefCountPtr<FVulkanImageSampler> Sampler = FRenderer::GetLinearSampler().As<FVulkanImageSampler>();
    
		VkDescriptorSet ImGuiImageID = ImGui_ImplVulkan_AddTexture(Sampler->GetSampler(), Image->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		ImGuiX::FImGuiImageInfo NewInfo;
        NewInfo.Image = Image;
		NewInfo.Size.x = (float)Image->GetSpecification().Extent.x;
		NewInfo.Size.y = (float)Image->GetSpecification().Extent.y;
		NewInfo.ID = reinterpret_cast<ImTextureID>(ImGuiImageID);
		ImageCache.emplace(Image->GetGuid(), NewInfo);

		RegisterNewImage(Image->GetGuid(), NewInfo.ID, "ImGui_" + Image->GetFriendlyName());
    
		return NewInfo;
	}

    void FVulkanImGuiRender::DestroyImGuiTexture(const ImGuiX::FImGuiImageInfo& ImageInfo)
    {
    	
    }
	
	void FVulkanImGuiRender::DestroyImGuiTexture(const FRHIImage& Image)
    {
    	
    }
    
    void FVulkanImGuiRender::RegisterNewImage(FGuid Guid, ImTextureID NewImage, const FString& DebugName)
    {
    	Assert(ImageCache.find(Guid) != ImageCache.end());
    	
    	FVulkanRenderContext* VkRenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	VkDevice Device = VkRenderContext->GetDevice();

    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = DebugName.c_str();
    	NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
    	NameInfo.objectHandle = reinterpret_cast<uint64>(NewImage);
    	
    	VkRenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    void FVulkanImGuiRender::OnEndFrame()
    {
        FRenderer::Submit([&]
        {
			if(ImDrawData* DrawData = ImGui::GetDrawData())
			{
				TRefCountPtr<FVulkanCommandBuffer> Buffer = FRenderer::GetCommandBuffer().As<FVulkanCommandBuffer>();
				VkCommandBuffer CmdBuffer = Buffer->GetCommandBuffer();
            
				VkRenderingAttachmentInfo colorAttachment = {};
				colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
				colorAttachment.imageView = FRenderer::GetRenderContext()->GetSwapchain()->GetCurrentImage().As<FVulkanImage>()->GetImageView();
				colorAttachment.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				colorAttachment.clearValue = {1, 1, 1, };
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            
				VkRenderingInfo renderInfo = {};
				renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
				renderInfo.pColorAttachments = &colorAttachment;
				renderInfo.colorAttachmentCount = 1;
				renderInfo.renderArea.extent.height =	FRenderer::GetRenderContext()->GetSwapchain()->GetSpecs().Extent.Y;
				renderInfo.renderArea.extent.width =	FRenderer::GetRenderContext()->GetSwapchain()->GetSpecs().Extent.X;
				renderInfo.layerCount = 1;
            
				vkCmdBeginRendering(CmdBuffer, &renderInfo);
            
				ImGui_ImplVulkan_RenderDrawData(DrawData, CmdBuffer);
            
				vkCmdEndRendering(CmdBuffer);
				
			}
        });
    }

}
