
#include "ViewportLayer.h"
#include "ImGui/ImGuiRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneRenderer.h"

namespace Lumina
{
    void ViewportLayer::OnAttach()
    {
    }

    void ViewportLayer::OnDetach()
    {
    }

    void ViewportLayer::OnUpdate(double DeltaTime)
    {
        if (Scene.expired())
        {
            return;
        }
        
        if (std::shared_ptr<FSceneRenderer> SceneRenderer = Scene.lock()->GetSceneRenderer())
        {
            TRefPtr<FImage> RenderTarget = SceneRenderer->GetRenderTarget();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Viewport");
            ImGui::PopStyleVar();

            ImVec2 Max = ImGui::GetWindowContentRegionMax();
            ImVec2 Min = ImGui::GetWindowContentRegionMin();
            ImVec2 NewRegion;
            NewRegion.x = Max.x - Min.x;
            NewRegion.y = Max.y - Min.y;
            
            float AspectRatio = (NewRegion.x / NewRegion.y);
            Scene.lock()->GetCurrentCamera()->SetAspectRatio(AspectRatio);
            float t = (NewRegion.x - 500) / (1200 - 500);
            t = glm::clamp(t, 0.0f, 1.0f);
            float fov = glm::mix(120.0f, 50.0f, t);

            Scene.lock()->GetCurrentCamera()->SetFOV(fov);

            FImGuiRenderer::RenderImage(RenderTarget, FRenderer::GetLinearSampler(), NewRegion, 0, true);

            ImVec2 overlayPos = ImVec2(Min.x + 10, Min.y + 10);
            glm::vec3 cameraPosition = Scene.lock()->GetCurrentCamera()->GetPosition();

            std::string cameraPosText = "Camera Pos: (" + std::to_string(cameraPosition.x) + ", " + 
                                        std::to_string(cameraPosition.y) + ", " + std::to_string(cameraPosition.z) + ")";
        
            ImGui::SetCursorPos(overlayPos); 
            ImGui::TextUnformatted(cameraPosText.c_str());

            ImGui::End();    
        }
    }

    void ViewportLayer::OnEvent(FEvent& InEvent)
    {
    }
}