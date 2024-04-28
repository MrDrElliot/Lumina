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

    void FConsolePanel::OnEvent(FEvent& Event)
    {
    }

    void FConsolePanel::OnRemoved()
    {
    }

    void FConsolePanel::OnRender(double DeltaTime)
    {
    }
    

    void FConsolePanel::OnNewScene()
    {
        ImGui::Begin(Name.c_str());

        ImGui::End();
    }
}
