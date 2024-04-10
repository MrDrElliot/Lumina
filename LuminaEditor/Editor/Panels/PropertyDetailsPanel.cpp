#include "PropertyDetailsPanel.h"

#include "imgui.h"


namespace Lumina
{
    FPropertyDetailsPanel::FPropertyDetailsPanel()
    {
        Name = "Details";
    }

    FPropertyDetailsPanel::~FPropertyDetailsPanel()
    {
    }

    void FPropertyDetailsPanel::OnAdded()
    {
    }

    void FPropertyDetailsPanel::OnRemoved()
    {
    }

    void FPropertyDetailsPanel::OnRender()
    {
    }

    void FPropertyDetailsPanel::OnImGui()
    {
        ImGui::Begin(Name.c_str());

        ImGui::End();
    }

    void FPropertyDetailsPanel::OnNewScene()
    {
    }
}
