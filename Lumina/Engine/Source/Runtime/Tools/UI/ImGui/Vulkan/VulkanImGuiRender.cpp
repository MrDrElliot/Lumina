#include "VulkanImGuiRender.h"

#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/Engine/Engine.h"
#include "Core/Windows/Window.h"
#include "GUID/GUID.h"
#include "Renderer/RenderManager.h"
#include "Renderer/API/Vulkan/VulkanMacros.h"
#include "Renderer/API/Vulkan/VulkanRenderContext.h"
#include "Renderer/API/Vulkan/VulkanSwapchain.h"
#include "Scene/Scene.h"

namespace Lumina
{

    void FVulkanImGuiRender::Initialize(FSubsystemManager& Manager)
    {
		IImGuiRenderer::Initialize(Manager);
		
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
        PoolInfo.poolSizeCount = (uint32)std::size(PoolSizes);
        PoolInfo.pPoolSizes = PoolSizes;
		
		FRenderManager* RenderManager = Manager.GetSubsystem<FRenderManager>();

		VulkanRenderContext = RenderManager->GetRenderContext<FVulkanRenderContext>();
		
        VK_CHECK(vkCreateDescriptorPool(VulkanRenderContext->GetDevice()->GetDevice(), &PoolInfo, nullptr, &DescriptorPool));

        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = "ImGui Descriptor Pool";
    	NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
    	NameInfo.objectHandle = reinterpret_cast<uint64>(DescriptorPool);
		
		
        Assert(ImGui_ImplGlfw_InitForVulkan(Windowing::GetPrimaryWindowHandle()->GetWindow(), true));

		VkFormat Format = VulkanRenderContext->GetSwapchain()->GetSwapchainFormat();
		
        VkPipelineRenderingCreateInfo RenderPipeline = {};
        RenderPipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        RenderPipeline.pColorAttachmentFormats = &Format;
        RenderPipeline.colorAttachmentCount = 1;

    	
        ImGui_ImplVulkan_InitInfo InitInfo = {};
        InitInfo.PipelineRenderingCreateInfo = RenderPipeline;
        InitInfo.Instance = VulkanRenderContext->GetVulkanInstance();
        InitInfo.PhysicalDevice = VulkanRenderContext->GetDevice()->GetPhysicalDevice();
        InitInfo.Device = VulkanRenderContext->GetDevice()->GetDevice();
        InitInfo.Queue = VulkanRenderContext->GetQueue(ECommandQueue::Graphics)->Queue;
        InitInfo.DescriptorPool = DescriptorPool;
        InitInfo.MinImageCount = 2;
        InitInfo.ImageCount = 2;
        InitInfo.UseDynamicRendering = true;
        InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;


        Assert(ImGui_ImplVulkan_Init(&InitInfo))
    }

    void FVulkanImGuiRender::Deinitialize()
    {
    	for (auto& KVP : ImageCache)
    	{
    		ImGui_ImplVulkan_RemoveTexture(KVP.second);
    	}
    	
    	ImGui_ImplVulkan_Shutdown();
    	
    	vkDestroyDescriptorPool(VulkanRenderContext->GetDevice()->GetDevice(), DescriptorPool, nullptr);
    	
    	ImGui_ImplGlfw_Shutdown();
    	ImGui::DestroyContext();
    }

    void FVulkanImGuiRender::OnStartFrame(const FUpdateContext& UpdateContext)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
	
    void FVulkanImGuiRender::OnEndFrame(const FUpdateContext& UpdateContext)
    {
		if(ImDrawData* DrawData = ImGui::GetDrawData())
		{
			FRHICommandListRef CommandList = VulkanRenderContext->GetCommandList(ECommandQueue::Graphics);
			
			FRenderPassBeginInfo RenderPass; RenderPass
			.AddColorAttachment(GEngine->GetEngineViewport()->GetRenderTarget())
			.SetColorLoadOp(ERenderLoadOp::Clear)
			.SetColorStoreOp(ERenderLoadOp::Store)
			.SetColorClearColor(FColor(1.0f))
			.SetRenderArea(GEngine->GetEngineViewport()->GetRenderTarget()->GetDescription().Extent);
			
			CommandList->BeginRenderPass(RenderPass);
			
			ImGui_ImplVulkan_RenderDrawData(DrawData, CommandList->GetAPIResource<VkCommandBuffer>());

			CommandList->EndRenderPass();
		}
    }

    ImTextureID FVulkanImGuiRender::GetOrCreateImTexture(FRHIImageRef Image)
    {
	    VkImage VulkanImage = Image->GetAPIResource<VkImage>();
    	VkImageView VulkanImageView = Image->GetAPIResource<VkImageView, EAPIResourceType::ImageView>();
    	
    	auto It = ImageCache.find(VulkanImage);
		
    	if (It != ImageCache.end())
    	{
    		return (intptr_t)It->second;
    	}

    	FRHISamplerRef Sampler = GEngine->GetEngineSubsystem<FRenderManager>()->GetLinearSampler();
    	VkSampler VulkanSampler = Sampler->GetAPIResource<VkSampler>();
    	
    	VkDescriptorSet DescriptorSet = ImGui_ImplVulkan_AddTexture(VulkanSampler, VulkanImageView, VK_IMAGE_LAYOUT_GENERAL);
    	ImageCache.insert_or_assign(VulkanImage, DescriptorSet);

    	return (intptr_t)DescriptorSet;
    	
    }
}
