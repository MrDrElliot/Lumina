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
        FVulkanRenderContext* RenderContext = FRenderContext::Get<FVulkanRenderContext>();
        
        ImGui::Begin("Scene");

        ImVec2 Max = ImGui::GetWindowContentRegionMax();
        ImVec2 Min = ImGui::GetWindowContentRegionMin();
        ImVec2 NewRegion;
        NewRegion.x = Max.x - Min.x;
        NewRegion.y = Max.y - Min.y;
        
        ImGui::Text("Viewport Size: %i %i", (int)NewRegion.x, (int)NewRegion.y);
        
        ImGui::Image(RenderContext->GetActiveSwapChain()->GetDrawImage().ImGuiTexture, NewRegion);
        
        ImGui::End();
    }
}
