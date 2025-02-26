#include "VulkanImGuiRender.h"

#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/Application/Application.h"
#include "Core/Windows/Window.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RHIIncl.h"
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

		FVulkanRenderContext* VulkanBackend = RenderManager->GetRenderContext<FVulkanRenderContext>();
		
        VK_CHECK(vkCreateDescriptorPool(VulkanBackend->GetDevice(), &PoolInfo, nullptr, &DescriptorPool));

        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = "ImGui Descriptor Pool";
    	NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
    	NameInfo.objectHandle = reinterpret_cast<uint64>(DescriptorPool);
		
		
        Assert(ImGui_ImplGlfw_InitForVulkan(Windowing::GetPrimaryWindowHandle()->GetWindow(), true));

		VkFormat Format = VulkanBackend->GetSwapchain()->GetSwapchainFormat();
		
        VkPipelineRenderingCreateInfo RenderPipeline = {};
        RenderPipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        RenderPipeline.pColorAttachmentFormats = &Format;
        RenderPipeline.colorAttachmentCount = 1;

    	
        ImGui_ImplVulkan_InitInfo InitInfo = {};
        InitInfo.PipelineRenderingCreateInfo = RenderPipeline;
        InitInfo.Instance = VulkanBackend->GetVulkanInstance();
        InitInfo.PhysicalDevice = VulkanBackend->GetPhysicalDevice();
        InitInfo.Device = VulkanBackend->GetDevice();
        InitInfo.Queue = VulkanBackend->GetCommandQueues().GraphicsQueue;
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
    	/*for (auto& KVP : ImageCache)
    	{
    		ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(KVP.second.ID));
    	}*/
    	
    	ImGui_ImplVulkan_Shutdown();
    	
    	vkDestroyDescriptorPool(VulkanRenderContext->GetDevice(), DescriptorPool, nullptr);
    	
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
			VulkanRenderContext = UpdateContext.GetSubsystem<FRenderManager>()->GetRenderContext<FVulkanRenderContext>();
			
			FVulkanCommandList* VulkanCommandList = VulkanRenderContext->GetPrimaryCommandList();

			FRHIImageHandle Handle = VulkanRenderContext->GetSwapchain()->GetCurrentImage();
			
			FRenderPassBeginInfo RenderPass; RenderPass
			.AddColorAttachment(Handle)
			.SetColorLoadOp(ERenderLoadOp::Clear)
			.SetColorStoreOp(ERenderLoadOp::Store)
			.SetColorClearColor(FColor(1.0f))
			.SetRenderArea(VulkanRenderContext->GetSwapchain()->GetSwapchainExtent());

			
			VulkanRenderContext->BeginRenderPass(VulkanCommandList, RenderPass);
			
			ImGui_ImplVulkan_RenderDrawData(DrawData, VulkanCommandList->CommandBuffer);

			VulkanRenderContext->EndRenderPass(VulkanCommandList);
           

		}
    }

    void FVulkanImGuiRender::RegisterNewImage(FGuid Guid, ImTextureID NewImage, const FString& DebugName)
    {
    }
}
