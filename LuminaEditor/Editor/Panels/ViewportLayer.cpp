#include "ViewportLayer.h"

#include "ImGui/ImGuiRenderer.h"
#include <ImGuizmo.h>

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
        if (std::shared_ptr<FSceneRenderer> SceneRenderer = Scene->GetSceneRenderer())
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
            Scene->GetCurrentCamera()->SetAspectRatio(AspectRatio);
            float t = (NewRegion.x - 500) / (1200 - 500);
            t = glm::clamp(t, 0.0f, 1.0f);
            float fov = glm::mix(120.0f, 50.0f, t);

            Scene->GetCurrentCamera()->SetFOV(fov);

            FImGuiRenderer::RenderImage(RenderTarget, FRenderer::GetLinearSampler(), NewRegion, 0, true);

            // Camera position overlay in the top-left corner
            ImVec2 overlayPos = ImVec2(Min.x + 10, Min.y + 10); // Slight padding from the window edge
            glm::vec3 cameraPosition = Scene->GetCurrentCamera()->GetPosition(); // Assuming GetPosition() returns the camera's position

            // Display the camera position in the top-left corner
            std::string cameraPosText = "Camera Pos: (" + std::to_string(cameraPosition.x) + ", " + 
                                        std::to_string(cameraPosition.y) + ", " + std::to_string(cameraPosition.z) + ")";
        
            ImGui::SetCursorPos(overlayPos); // Set the cursor position to the top-left corner
            ImGui::TextUnformatted(cameraPosText.c_str()); // Display the camera position text

            ImGui::End();    
        }
    }

    void ViewportLayer::OnEvent(FEvent& InEvent)
    {
    }
}