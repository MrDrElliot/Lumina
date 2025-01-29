#include "OpenProjectPanel.h"

#include "PanelManager.h"
#include "Project/Project.h"

namespace Lumina
{
    void OpenProjectPanel::OnAttach()
    {
        FileBrowser.SetTitle("Select Project");
        FileBrowser.SetTypeFilters({".lproject"});
    }

    void OpenProjectPanel::OnDetach()
    {
    }

void OpenProjectPanel::OnUpdate(double DeltaTime)
{
    // Set up ImGui window properties
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Select Project", nullptr, windowFlags))
    {
        // Center the "Select Project" button
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 120) * 0.5f);
        if (ImGui::Button("Select Project", ImVec2(120, 30)))
        {
            FileBrowser.Open();
        }

        // Show file browser and handle selected file
        FileBrowser.Display();
        static std::string selectedFilePath = "";
        if (FileBrowser.HasSelected())
        {
            selectedFilePath = FileBrowser.GetSelected().string();  // Get selected file path as string
            FileBrowser.ClearSelected();  // Clear selection to prevent re-triggering
        }

        // Add spacing and display the selected file path as read-only text
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Selected File:");
        ImGui::SameLine();
        ImGui::TextWrapped(selectedFilePath.c_str()); // Display file path

        // Center the "Confirm" and "Cancel" buttons
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 220) * 0.5f);
        
        if (ImGui::Button("Confirm", ImVec2(100, 30)) && !selectedFilePath.empty())
        {
            Project::Load(selectedFilePath);
            PanelManager::Get()->GetPanel<OpenProjectPanel>()->SetVisible(false);
            ImGui::CloseCurrentPopup();  // Close the window
        }
        
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(100, 30)))
        {
            selectedFilePath.clear();  // Clear selected path on cancel
            PanelManager::Get()->GetPanel<OpenProjectPanel>()->SetVisible(false);
            ImGui::CloseCurrentPopup();  // Close the window
        }
    }

    ImGui::End();
}


    void OpenProjectPanel::OnEvent(FEvent& InEvent)
    {
    }
}
