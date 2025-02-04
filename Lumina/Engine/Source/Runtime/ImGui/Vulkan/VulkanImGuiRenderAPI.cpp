#include "VulkanImGuiRenderAPI.h"
#include "Renderer/Pipeline.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/Application/Application.h"
#include "Core/Windows/Window.h"
#include "GUID/GUID.h"
#include "ImGui/ImGuiFonts.h"
#include "ImGui/ImGuiRenderer.h"
#include "Renderer/RHI/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/RHI/Vulkan/VulkanImage.h"
#include "Renderer/RHI/Vulkan/VulkanRenderContext.h"
#include "Renderer/RHI/Vulkan/VulkanSwapchain.h"
#include "Scene/Scene.h"
#include "Scene/SceneRenderer.h"

namespace Lumina
{

    static VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;

    static eastl::unordered_map<FGuid, VkDescriptorSet> ImGuiImageDescriptorSets;
    
    FVulkanImGuiRenderAPI::FVulkanImGuiRenderAPI()
    {

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


        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows


        
        Font::Add("Resources/Fonts/Roboto/Roboto-Light.ttf", "Roboto-Light");
        Font::Add("Resources/Fonts/Roboto/Roboto-Medium.ttf", "Roboto-Medium");
        Font::Add("Resources/Fonts/Roboto/Roboto-Bold.ttf", "Roboto-Bold");
        Font::Add("Resources/Fonts/Roboto/Roboto-Italic.ttf", "Roboto-Italic");
        
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
    	
        style.Alpha = 1.0f;
        style.Colors[ImGuiCol_Text] =                   ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] =           ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] =               ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] =                ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] =                ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_Border] =                 ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] =           ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] =                ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] =         ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] =          ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_TitleBg] =                ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] =          ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] =       ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] =              ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] =            ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] =          ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] =   ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] =    ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] =              ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] =             ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] =       ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        style.Colors[ImGuiCol_Button] =                 ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] =          ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] =           ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Header] =                 ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] =          ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] =           ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Separator] =              style.Colors[ImGuiCol_Border];
        style.Colors[ImGuiCol_SeparatorHovered] =       ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive] =        ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_ResizeGrip] =             ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] =      ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] =       ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_Tab] =                    ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        style.Colors[ImGuiCol_TabHovered] =             ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        style.Colors[ImGuiCol_TabActive] =              ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] =           ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] =     ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_DockingPreview] =         ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        style.Colors[ImGuiCol_DockingEmptyBg] =         ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] =              ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] =       ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] =          ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] =   ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] =         ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] =         ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_NavHighlight] =           ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] =  ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] =      ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] =       ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        style.GrabRounding = style.FrameRounding =      2.3f;

    	FVulkanRenderContext* VkRenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	VkDevice Device = VkRenderContext->GetDevice();
    	
        vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &DescriptorPool);

        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = "ImGui Descriptor Pool";
    	NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
    	NameInfo.objectHandle = reinterpret_cast<uint64_t>(DescriptorPool);
    	
    	VkRenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
        
        ImGui_ImplGlfw_InitForVulkan(FApplication::Get().GetWindow()->GetWindow(), true);

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

    	
        ImGui_ImplVulkan_Init(&InitInfo);
        ImGui_ImplVulkan_CreateFontsTexture();

    }

    FVulkanImGuiRenderAPI::~FVulkanImGuiRenderAPI()
    {
	    for (auto Set : ImGuiImageDescriptorSets)
	    {
    		ImGui_ImplVulkan_RemoveTexture(Set.second);
	    }
    	
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void FVulkanImGuiRenderAPI::BeginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
	
    void FVulkanImGuiRenderAPI::RenderImage(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer, bool bFlip)
    {
        TRefPtr<FVulkanImage> VkImage = RefPtrCast<FVulkanImage>(Image);
        TRefPtr<FVulkanImageSampler> VkSampler = RefPtrCast<FVulkanImageSampler>(Sampler);
        if (ImGuiImageDescriptorSets.find(Image->GetGuid()) == ImGuiImageDescriptorSets.end())
        {
        	
            VkDescriptorSet ImGuiImageID = ImGui_ImplVulkan_AddTexture
        	(
                VkSampler->GetSampler(),
                VkImage->GetImageView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );

        	FVulkanRenderContext* VkRenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
        	VkDevice Device = VkRenderContext->GetDevice();
        	
        	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
        	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        	NameInfo.pObjectName = Image->GetFriendlyName().c_str();
        	NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
        	NameInfo.objectHandle = reinterpret_cast<uint64_t>(ImGuiImageID);
    	
        	VkRenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
        	
            ImGuiImageDescriptorSets.emplace(Image->GetGuid(), ImGuiImageID);
        }
        ImGui::Image(ImGuiImageDescriptorSets[Image->GetGuid()], Size, { 0, (float)!bFlip }, { 1, (float)bFlip });
    }

    ImTextureID FVulkanImGuiRenderAPI::CreateImGuiTexture(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer, bool bFlip)
    {
    	TRefPtr<FVulkanImage> VkImage = RefPtrCast<FVulkanImage>(Image);
    	TRefPtr<FVulkanImageSampler> VkSampler = RefPtrCast<FVulkanImageSampler>(Sampler);
    	if (ImGuiImageDescriptorSets.find(Image->GetGuid()) == ImGuiImageDescriptorSets.end())
    	{
    		VkDescriptorSet ImGuiImageID = ImGui_ImplVulkan_AddTexture(
				VkSampler->GetSampler(),
				VkImage->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);
    		ImGuiImageDescriptorSets.emplace(Image->GetGuid(), ImGuiImageID);

    		FVulkanRenderContext* VkRenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    		VkDevice Device = VkRenderContext->GetDevice();

    		VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    		NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    		NameInfo.pObjectName = Image->GetFriendlyName().c_str();
    		NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
    		NameInfo.objectHandle = reinterpret_cast<uint64_t>(ImGuiImageID);
    	
    		VkRenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
    	}

    	return ImGuiImageDescriptorSets[Image->GetGuid()];
    }

    void FVulkanImGuiRenderAPI::EndFrame()
    {
        ImGuiIO& Io = ImGui::GetIO();
        Io.DisplaySize.x = (float)FRenderer::GetRenderContext()->GetSwapchain()->GetSpecs().Extent.x;
    	Io.DisplaySize.y = (float)FRenderer::GetRenderContext()->GetSwapchain()->GetSpecs().Extent.y;

        ImGui::Render();
    	
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        FRenderer::Submit([&]
        {
			if(ImDrawData* DrawData = ImGui::GetDrawData())
			{
				TRefPtr<FVulkanCommandBuffer> Buffer = RefPtrCast<FVulkanCommandBuffer>(FRenderer::GetCommandBuffer());
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
				renderInfo.renderArea.extent.height =	FRenderer::GetRenderContext()->GetSwapchain()->GetSpecs().Extent.y;
				renderInfo.renderArea.extent.width =	FRenderer::GetRenderContext()->GetSwapchain()->GetSpecs().Extent.x;
				renderInfo.layerCount = 1;
            
				vkCmdBeginRendering(CmdBuffer, &renderInfo);
            
				ImGui_ImplVulkan_RenderDrawData(DrawData, CmdBuffer);
            
				vkCmdEndRendering(CmdBuffer);
			}
        });
    }

    void FVulkanImGuiRenderAPI::Init()
    {
    }

    void FVulkanImGuiRenderAPI::Shutdown()
    {
    }
}
