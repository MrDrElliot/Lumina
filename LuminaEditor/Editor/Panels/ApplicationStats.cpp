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

        ImGui::Begin("Application Stats");

        ImGui::SeparatorText("Application Stats");
        
        ImGui::Text("Frame Count: %llu", Stats.FrameCount);
        ImGui::Text("FPS: %u", Stats.FPS);
        ImGui::Text("Delta Time: %.3f ms", Stats.DeltaTime);
        
        ImGui::SeparatorText("Render Stats");
        
        ImGui::Text("Number of Draw Calls: %u", FRenderer::sInternalData.NumDrawCalls);
        ImGui::Text("Number of Vertices: %u", FRenderer::sInternalData.NumVertices);
        
        ImGui::End();
    }

    void ApplicationStats::OnEvent(FEvent& InEvent)
    {
    }
}
