#include "VulkanImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Source/Runtime/Log/Log.h"

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
        ImGui::Render();
        ImGui::UpdatePlatformWindows();
    }

    void FVulkanImGuiLayer::OnAttach()
    {
        ImGui::CreateContext();
        ImGuiIO& Io = ImGui::GetIO(); (void)Io;
        Io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
       // Io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        Io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();
        SetupDarkThemeColors();

        ImGuiStyle& Style = ImGui::GetStyle();
        if(Io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            Style.WindowRounding = 7.5f;
            Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, Style.Colors[ImGuiCol_WindowBg].w);

        
    }

    void FVulkanImGuiLayer::OnDetach()
    {
        FImGuiLayer::OnDetach();
    }

    void FVulkanImGuiLayer::OnImGuiRender()
    {
        ImGui::Begin("BTest");
        ImGui::ShowAboutWindow();
        ImGui::End();


    }
}
