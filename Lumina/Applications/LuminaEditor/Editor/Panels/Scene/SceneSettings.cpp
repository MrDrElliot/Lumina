#include "SceneSettings.h"

#include "imgui.h"
#include "Core/Application/Application.h"
#include "Scene/Scene.h"


namespace Lumina
{
    void SceneSettings::OnAttach()
    {
    }

    void SceneSettings::OnDetach()
    {
    }

    void SceneSettings::OnUpdate(double DeltaTime)
    {
        FSceneSettings& Settings = FApplication::GetActiveScene()->GetSceneSettings();

        // Begin the Scene Settings ImGui window with some flags for appearance
        ImGui::Begin("Scene Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

        // Header for Background Settings
        ImGui::Text("Background Settings");
        ImGui::Separator();
        ImGui::Spacing();

        // Background color setting with a tooltip for additional information
        ImGui::ColorEdit3("Background Color", (float*)&Settings.BackgroundColor);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Choose the background color for the scene.");

        ImGui::Spacing();

        // Header for Grid Settings
        ImGui::Text("Grid Settings");
        ImGui::Separator();
        ImGui::Spacing();

        // Grid visibility setting with a tooltip
        ImGui::Checkbox("Show Grid", &Settings.bShowGrid);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Toggle the visibility of the infinite grid.");

        // Add any other settings below in a similar structured format...

        // End window
        ImGui::End();
    }

    
    void SceneSettings::OnEvent(FEvent& InEvent)
    {
    }
}
