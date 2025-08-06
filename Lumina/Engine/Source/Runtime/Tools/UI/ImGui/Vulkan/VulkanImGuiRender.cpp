#include "VulkanImGuiRender.h"

#include "ImGuizmo.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Core/Windows/Window.h"
#include "Renderer/RenderManager.h"
#include "Renderer/API/Vulkan/VulkanMacros.h"
#include "Renderer/API/Vulkan/VulkanRenderContext.h"
#include "Renderer/API/Vulkan/VulkanSwapchain.h"
#include "Scene/Scene.h"

namespace Lumina
{
	FString VkFormatToString(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_B8G8R8A8_SRGB: return "VK_FORMAT_B8G8R8A8_SRGB";
		case VK_FORMAT_R8G8B8A8_SRGB: return "VK_FORMAT_R8G8B8A8_SRGB";
		case VK_FORMAT_B8G8R8A8_UNORM: return "VK_FORMAT_B8G8R8A8_UNORM";
		case VK_FORMAT_R8G8B8A8_UNORM: return "VK_FORMAT_R8G8B8A8_UNORM";
		case VK_FORMAT_R32G32B32A32_SFLOAT: return "VK_FORMAT_R32G32B32A32_SFLOAT";
		case VK_FORMAT_R32G32B32_SFLOAT: return "VK_FORMAT_R32G32B32_SFLOAT";
		case VK_FORMAT_R32G32_SFLOAT: return "VK_FORMAT_R32G32_SFLOAT";
		case VK_FORMAT_R32_SFLOAT: return "VK_FORMAT_R32_SFLOAT";
		default: return "Unknown Format";
		}
	}
	
	FString VkColorSpaceToString(VkColorSpaceKHR colorSpace)
	{
		switch (colorSpace)
		{
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR";
		case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT: return "VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT";
		case VK_COLOR_SPACE_BT2020_LINEAR_EXT: return "VK_COLOR_SPACE_BT2020_LINEAR_EXT";
		default: return "Unknown ColorSpace";
		}
	}
	
    void FVulkanImGuiRender::Initialize(FSubsystemManager& Manager)
    {
		IImGuiRenderer::Initialize(Manager);
    	LUMINA_PROFILE_SCOPE();

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
    	
        VulkanRenderContext->SetVulkanObjectName("ImGui Descriptor Pool", VK_OBJECT_TYPE_DESCRIPTOR_POOL, reinterpret_cast<uint64>(DescriptorPool));
    	
        Assert(ImGui_ImplGlfw_InitForVulkan(Windowing::GetPrimaryWindowHandle()->GetWindow(), true));

		VkFormat Format = VulkanRenderContext->GetSwapchain()->GetSwapchainFormat();
		
        VkPipelineRenderingCreateInfo RenderPipeline = {};
        RenderPipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        RenderPipeline.pColorAttachmentFormats = &Format;
        RenderPipeline.colorAttachmentCount = 1;

    	
        ImGui_ImplVulkan_InitInfo InitInfo = {};
    	InitInfo.ApiVersion = VK_API_VERSION_1_3;
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
    	VulkanRenderContext->WaitIdle();
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
    	LUMINA_PROFILE_SCOPE();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    	ImGuizmo::BeginFrame();
    }
	
    void FVulkanImGuiRender::OnEndFrame(const FUpdateContext& UpdateContext)
    {
    	LUMINA_PROFILE_SCOPE();
    	
		if(ImDrawData* DrawData = ImGui::GetDrawData())
		{
			FRHICommandListRef CommandList = VulkanRenderContext->GetCommandList(ECommandQueue::Graphics);
			
			FRenderPassBeginInfo RenderPass; RenderPass
			.AddColorAttachment(GEngine->GetEngineViewport()->GetRenderTarget())
			.SetColorLoadOp(ERenderLoadOp::Load)
			.SetColorStoreOp(ERenderStoreOp::Store)
			.SetColorClearColor(FColor::Black)
			.SetRenderArea(GEngine->GetEngineViewport()->GetRenderTarget()->GetDescription().Extent);
			
			CommandList->BeginRenderPass(RenderPass);
			
			ImGui_ImplVulkan_RenderDrawData(DrawData, CommandList->GetAPIResource<VkCommandBuffer>());

			CommandList->EndRenderPass();
		}
    }

	void FVulkanImGuiRender::DrawRenderDebugInformationWindow(bool* bOpen, const FUpdateContext& Context)
	{
		if (!ImGui::Begin("Vulkan Render Information", bOpen, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}
		VkPhysicalDevice physicalDevice = VulkanRenderContext->GetDevice()->GetPhysicalDevice();
	
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(physicalDevice, &props);
	
		VkPhysicalDeviceMemoryProperties memProps{};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

    	VmaAllocator Allocator = VulkanRenderContext->GetDevice()->GetAllocator()->GetAllocator();
    	
		ImGui::SeparatorText("Device Properties");
	
		if (ImGui::BeginTable("VulkanDeviceInfo", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			auto Label = [](const char* name, auto value)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(name);
				ImGui::TableSetColumnIndex(1); ImGui::Text("%s", value);
			};
	
			Label("Device Name", props.deviceName);
			Label("Device Type", 
				props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Discrete GPU" :
				props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "Integrated GPU" :
				props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "CPU" : "Other");
	
			Label("Vendor ID", std::format("0x{:04X}", props.vendorID).c_str());
			Label("Device ID", std::format("0x{:04X}", props.deviceID).c_str());
	
			Label("API Version", std::format("{}.{}.{}", 
				VK_VERSION_MAJOR(props.apiVersion), 
				VK_VERSION_MINOR(props.apiVersion), 
				VK_VERSION_PATCH(props.apiVersion)).c_str());
	
			Label("Driver Version", std::format("0x{:X}", props.driverVersion).c_str());
	
			Label("Max Image Dimension 2D", std::format("{}", props.limits.maxImageDimension2D).c_str());
			Label("Uniform Buffer Alignment", std::format("{} bytes", props.limits.minUniformBufferOffsetAlignment).c_str());
			Label("Storage Buffer Alignment", std::format("{} bytes", props.limits.minStorageBufferOffsetAlignment).c_str());
			Label("Total Allocated RHI Resources", std::format("{}", GTotalRenderResourcesAllocated).c_str());
			
			ImGui::EndTable();
		}

    	
    	ImGui::Spacing();
    	ImGui::SeparatorText("Swapchain Info");

		if (FVulkanSwapchain* Swapchain = VulkanRenderContext->GetSwapchain())
    	{
    		const VkSurfaceFormatKHR& surfaceFormat = Swapchain->GetSurfaceFormat();
    		const FIntVector2D& extent = Swapchain->GetSwapchainExtent();
    		VkPresentModeKHR presentMode = Swapchain->GetPresentMode();
			uint32 FramesInFlight = Swapchain->GetNumFramesInFlight();
    		uint32 imageCount = Swapchain->GetImageCount();
    		uint32 currentImageIndex = Swapchain->GetCurrentImageIndex();

    		if (ImGui::BeginTable("SwapchainTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    		{
    			auto Row = [](const char* label, const std::string& value)
    			{
    				ImGui::TableNextRow();
    				ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(label);
    				ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(value.c_str());
    			};

    			Row("Resolution", std::format("{}x{}", extent.X, extent.Y));
    			Row("Frames In Flight", std::format("{}", FramesInFlight));
    			Row("Image Count", std::format("{}", imageCount));
    			Row("Current Image Index", std::format("{}", currentImageIndex));
    			Row("Present Mode", std::format("{}", presentMode == VK_PRESENT_MODE_FIFO_KHR ? "FIFO (VSync)"
												   : presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? "Mailbox (Triple Buffer)"
												   : presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR ? "Immediate (No VSync)"
												   : "Unknown"));
    			Row("Format", std::format("{}", VkFormatToString(surfaceFormat.format).c_str()));
    			Row("Color Space", std::format("{}", VkColorSpaceToString(surfaceFormat.colorSpace).c_str()));

    			ImGui::EndTable();
    		}
    	}
    	else
    	{
    		ImGui::Text("No swapchain available.");
    	}  	
	
		ImGui::Spacing();
		ImGui::SeparatorText("Memory Heaps and Types");
	
		if (ImGui::BeginTable("VulkanMemoryInfo", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Heap");
			ImGui::TableSetupColumn("Size (MB)");
			ImGui::TableSetupColumn("Flags");
			ImGui::TableHeadersRow();
	
			for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
			{
				const VkMemoryHeap& heap = memProps.memoryHeaps[i];
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Heap %u", i);
				ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f MB", heap.size / (1024.0f * 1024.0f));
				ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted(
					heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "Device Local" : "Host Visible");
			}
	
			ImGui::EndTable();
		}
	

		ImGui::Spacing();
		ImGui::SeparatorText("Memory Usage (VMA)");
		
		// VMA Stats
		VmaTotalStatistics stats{};
		vmaCalculateStatistics(Allocator, &stats);
		
		ImGui::Spacing();

    	if (ImGui::BeginTable("VmaHeapDetails", 10, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
    	{
    		ImGui::TableSetupColumn("Heap");
    		ImGui::TableSetupColumn("Blocks");
    		ImGui::TableSetupColumn("Allocs");
    		ImGui::TableSetupColumn("Used (MB)");
    		ImGui::TableSetupColumn("Unused (MB)");
    		ImGui::TableSetupColumn("Min Alloc (KB)");
    		ImGui::TableSetupColumn("Max Alloc (MB)");
    		ImGui::TableSetupColumn("Free Ranges");
    		ImGui::TableSetupColumn("Min Empty (KB)");
    		ImGui::TableSetupColumn("Max Empty (MB)");
    		ImGui::TableHeadersRow();

    		for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i)
    		{
    			const VmaDetailedStatistics& heap = stats.memoryHeap[i];
    			if (heap.statistics.blockCount == 0 && heap.statistics.allocationCount == 0)
    				continue;

    			ImGui::TableNextRow();
    			ImGui::TableSetColumnIndex(0); ImGui::Text("Heap %u", i);
    			ImGui::TableSetColumnIndex(1); ImGui::Text("%u", heap.statistics.blockCount);
    			ImGui::TableSetColumnIndex(2); ImGui::Text("%u", heap.statistics.allocationCount);
    			ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", heap.statistics.allocationBytes / (1024.0f * 1024.0f)); // used
    			ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", (heap.statistics.blockBytes - heap.statistics.allocationBytes) / (1024.0f * 1024.0f));
    			ImGui::TableSetColumnIndex(5); ImGui::Text(heap.allocationSizeMin == VK_WHOLE_SIZE ? "-" : "%.2f", heap.allocationSizeMin / 1024.0f);
    			ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", heap.allocationSizeMax / (1024.0f * 1024.0f));
    			ImGui::TableSetColumnIndex(7); ImGui::Text("%u", heap.unusedRangeCount);
    			ImGui::TableSetColumnIndex(8); ImGui::Text(heap.unusedRangeSizeMin == VK_WHOLE_SIZE ? "-" : "%.2f", heap.unusedRangeSizeMin / 1024.0f);
    			ImGui::TableSetColumnIndex(9); ImGui::Text("%.2f", heap.unusedRangeSizeMax / (1024.0f * 1024.0f));

    		}
    		ImGui::EndTable();
    	}

		ImGui::Spacing();
	
		if (ImGui::Button("Close"))
			*bOpen = false;
	
		ImGui::End();
	}


    ImTextureID FVulkanImGuiRender::GetOrCreateImTexture(FRHIImageRef Image)
    {
    	LUMINA_PROFILE_SCOPE();
    	
    	if(Image == nullptr)
    	{
    		return 0;
    	}
    	
	    VkImage VulkanImage = Image->GetAPIResource<VkImage>();
    	VkImageView VulkanImageView = Image->GetAPIResource<VkImageView, EAPIResourceType::ImageView>();
    	
    	auto It = ImageCache.find(VulkanImage);
		
    	if (It != ImageCache.end())
    	{
    		return (intptr_t)It->second;
    	}

    	FRHISamplerRef Sampler = GEngine->GetEngineSubsystem<FRenderManager>()->GetLinearSampler();
    	VkSampler VulkanSampler = Sampler->GetAPIResource<VkSampler>();
    	
    	VkDescriptorSet DescriptorSet = ImGui_ImplVulkan_AddTexture(VulkanSampler, VulkanImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    	ImageCache.insert_or_assign(VulkanImage, DescriptorSet);

    	return (intptr_t)DescriptorSet;
    	
    }
}
