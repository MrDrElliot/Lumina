#include "SceneViewPanel.h"

#include "imgui.h"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/ImGui/ImGuiRenderer.h"
#include "Source/Runtime/Scene/Scene.h"
#include "Source/Runtime/Scene/SceneRenderer.h"

namespace Lumina
{
    class FImage;

    FSceneViewPanel::FSceneViewPanel(std::shared_ptr<LScene> InScene)
        :Scene(InScene)
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

    void FSceneViewPanel::OnRender(double DeltaTime)
    {
        if(std::shared_ptr<FSceneRenderer> SceneRenderer = Scene->GetSceneRenderer())
        {
            std::shared_ptr<FImage> RenderTarget = SceneRenderer->GetRenderTarget();
            ImGui::Begin("Viewport");

            ImVec2 Max = ImGui::GetWindowContentRegionMax();
            ImVec2 Min = ImGui::GetWindowContentRegionMin();
            ImVec2 NewRegion;
            NewRegion.x = Max.x - Min.x;
            NewRegion.y = Max.y - Min.y;

            ImGui::Text("Viewport Size: %i %i", (int)NewRegion.x, (int)NewRegion.y);

            float AspectRatio = (NewRegion.x / NewRegion. y);
            Scene->GetEditorCamera()->SetAspectRatio(AspectRatio);
            float t = (NewRegion.x - 500) / (1200 - 500);
            t = glm::clamp(t, 0.0f, 1.0f);
            float fov = glm::mix(120.0f, 50.0f, t);
            ImGui::Text("FOV: %f", fov);

            Scene->GetEditorCamera()->SetFOV(fov);

            FImGuiRenderer::RenderImage(RenderTarget, SceneRenderer->GetLinearSampler(), NewRegion, 0);

            
            ImGui::End();    
        }
        
    }
    
}
