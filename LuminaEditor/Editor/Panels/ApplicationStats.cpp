#include "ApplicationStats.h"

#include "imgui.h"
#include "Core/Application.h"
#include "Renderer/Renderer.h"

namespace Lumina
{
    void ApplicationStats::OnAttach()
    {
    }

    void ApplicationStats::OnDetach()
    {
    }

    void ApplicationStats::OnUpdate(double DeltaTime)
    {
        FApplicationStats Stats = FApplication::GetStats();

        // Create a new ImGui window
        ImGui::Begin("Application Stats");

        ImGui::SeparatorText("Application Stats");


        // Display frame count
        ImGui::Text("Frame Count: %d", Stats.FrameCount);

        // Display current frame time
        ImGui::Text("Frame Time: %.3f ms", Stats.CurrentFrameTime * 1000.0f);  // Convert seconds to milliseconds

        // Display FPS
        ImGui::Text("FPS: %u", Stats.FPS);

        ImGui::Text("Delta Time: %.3f ms", Stats.DeltaTime);
    
        // Display time since last frame
        ImGui::Text("Last Frame Time: %.3f ms", Stats.LastFrameTime * 1000.0f);

        

        ImGui::SeparatorText("Render Stats");
        
        ImGui::Text("Number of Draw Calls: %u", FRenderer::sInternalData.NumDrawCalls);

        ImGui::Text("Number of Vertices: %u", FRenderer::sInternalData.NumVertices);

        ImGui::End();
    }

    void ApplicationStats::OnEvent(FEvent& InEvent)
    {
    }
}
