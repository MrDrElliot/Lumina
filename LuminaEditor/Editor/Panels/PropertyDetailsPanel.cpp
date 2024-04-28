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

    void FPropertyDetailsPanel::OnEvent(FEvent& Event)
    {
    }

    void FPropertyDetailsPanel::OnAdded()
    {
    }

    void FPropertyDetailsPanel::OnRemoved()
    {
    }

    void FPropertyDetailsPanel::OnRender(double DeltaTime)
    {
        ImGui::Begin(Name.c_str());

        ImGui::End();
    }


    void FPropertyDetailsPanel::OnNewScene()
    {
    
    }
}
