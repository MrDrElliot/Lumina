#include "ConsolePanel.h"

#include "imgui.h"


namespace Lumina
{
    FConsolePanel::FConsolePanel()
    {
        Name = "Console";
    }

    FConsolePanel::~FConsolePanel()
    {
    }

    void FConsolePanel::OnAdded()
    {
    }

    void FConsolePanel::OnRemoved()
    {
    }

    void FConsolePanel::OnRender()
    {
    }

    void FConsolePanel::OnImGui()
    {
        ImGui::Begin(Name.c_str());

        ImGui::End();
    }

    void FConsolePanel::OnNewScene()
    {
    }
}
