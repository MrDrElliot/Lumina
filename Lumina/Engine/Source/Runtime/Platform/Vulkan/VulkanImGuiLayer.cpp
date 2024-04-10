#include "VulkanImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "GLFW/glfw3.h"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"
#include "Source/Runtime/ImGui/ImGuiFonts.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanHelpers.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
    void FVulkanImGuiLayer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void FVulkanImGuiLayer::End()
    {
        ImGuiIO& Io = ImGui::GetIO();
        FApplication& App = FApplication::Get();
        Io.DisplaySize = ImVec2(App.GetWindow().GetWidth(), App.GetWindow().GetHeight());
        
  		ImGui::Render();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
    }

    void FVulkanImGuiLayer::OnAttach()
    {

        FVulkanRenderContext* RenderContext = FRenderContext::Get<FVulkanRenderContext>();

        
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
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows


        
        Font::Add("Resources/Fonts/Roboto/Roboto-Light.ttf", "Roboto-Light");
        Font::Add("Resources/Fonts/Roboto/Roboto-Medium.ttf", "Roboto-Medium");
        Font::Add("Resources/Fonts/Roboto/Roboto-Bold.ttf", "Roboto-Bold");
        Font::Add("Resources/Fonts/Roboto/Roboto-Italic.ttf", "Roboto-Italic");


        
        
        ImGui::StyleColorsDark();
        SetupDarkThemeColors();
        
        VkDescriptorPool ImGuiPool;
        VK_CHECK(vkCreateDescriptorPool(RenderContext->GetDevice(), &PoolInfo, nullptr, &ImGuiPool));
        
        ImGui_ImplGlfw_InitForVulkan(RenderContext->GetActiveSwapChain()->GetWindow()->GetWindow(), true);

        VkPipelineRenderingCreateInfo RenderPipeline = {};
        RenderPipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        RenderPipeline.pColorAttachmentFormats = &RenderContext->GetActiveSwapChain()->GetFormat();
        RenderPipeline.colorAttachmentCount = 1;

        ImGui_ImplVulkan_InitInfo InitInfo = {};
        InitInfo.PipelineRenderingCreateInfo = RenderPipeline;
        InitInfo.Instance = RenderContext->GetInstance();
        InitInfo.PhysicalDevice = RenderContext->GetDevice().physical_device;
        InitInfo.Device = RenderContext->GetDevice();
        InitInfo.Queue = RenderContext->GetGraphicsQueue();
        InitInfo.DescriptorPool = ImGuiPool;
        InitInfo.MinImageCount = 2;
        InitInfo.ImageCount = 2;
        InitInfo.UseDynamicRendering = true;
        InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;


        
        ImGui_ImplVulkan_Init(&InitInfo);
        ImGui_ImplVulkan_CreateFontsTexture();
        vkDeviceWaitIdle(RenderContext->GetDevice());
        ImGui_ImplVulkan_DestroyFontsTexture();


        
    }

    void FVulkanImGuiLayer::OnDetach()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void FVulkanImGuiLayer::OnImGuiRender()
    {
        
    }
}
