#include "EditorSettingsPanel.h"

#include "imgui.h"
#include "Project/PanelManager.h"
#include "Settings/EditorSettings.h"

namespace Lumina
{
    void FEditorSettingsPanel::OnAttach()
    {
    }

    void FEditorSettingsPanel::OnDetach()
    {
    }

    void FEditorSettingsPanel::OnUpdate(double DeltaTime)
    {
        bool bOpen = true;
        ImGui::Begin("Editor Settings", &bOpen, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Startup Project: %ls", FEditorSettings::Get()->GetStartupProject().c_str());
        
        ImGui::End();

        if(!bOpen)
        {
            PanelManager::Get()->GetPanel<FEditorSettingsPanel>()->SetVisible(false);
            FEditorSettings::Get()->Serialize();
        }
    }

    void FEditorSettingsPanel::OnEvent(FEvent& InEvent)
    {
    }

}
