#include "SceneViewPanel.h"

#include <complex.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_vulkan.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanHelpers.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
    FSceneViewPanel::FSceneViewPanel()
    {
        
    }

    FSceneViewPanel::~FSceneViewPanel()
    {
    }

    void FSceneViewPanel::OnNewScene()
    {
    }

    void FSceneViewPanel::OnAdded()
    {
    }

    void FSceneViewPanel::OnRemoved()
    {
    }

    void FSceneViewPanel::OnRender()
    {
    }

    void FSceneViewPanel::OnImGui()
    {
        ImGui::ShowDemoWindow();
        FVulkanRenderContext* Context = FApplication::Get().GetRenderContext<FVulkanRenderContext>();
        FAllocatedImage Image = Context->GetActiveSwapChain()->GetDrawImage();
        ImGui::Begin("Scene Viewport");
        ImVec2 Max = ImGui::GetWindowContentRegionMax();
        ImVec2 Min = ImGui::GetWindowContentRegionMin();
        ImVec2 Region;
        Region.x = Max.x - Min.x;
        Region.y = Max.y - Min.y;
        ImGui::Text("Viewport Size: %f, %f", Region.x, Region.y);

        if(ImTextureID ID = Image.ImGuiTexture)
        {
            ImGui::Image(ID, Region);
        }
        
        ImGui::End();
    }
}
