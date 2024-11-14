#include "NewProjectPanel.h"
#include "imgui.h"
#include <filesystem>

#include "PanelManager.h"
#include "Project/Project.h"

namespace Lumina
{
    void NewProjectPanel::OnAttach()
    {
        FileDialog.SetTitle("Select Asset to Import");
    }

    void NewProjectPanel::OnDetach()
    {
    }
    

    void NewProjectPanel::OnUpdate(double DeltaTime)
    {
        // Open the modal window if it's not already open
        if (ImGui::Begin("New Project", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::OpenPopup("New Project Popup"); // Open the modal popup
    
            // Modal settings: Ensure it's centered and auto-sizing
            ImVec2 windowSize = ImVec2(0, 0);  // Let ImGui auto-size
            ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x - 400) * 0.5f, (ImGui::GetIO().DisplaySize.y - 300) * 0.5f);
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);  // Center the window on the screen
            ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always); // Let ImGui auto-size the window based on content
    
            if (ImGui::BeginPopupModal("New Project Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                // Create input fields for the project name and project directory
                ImGui::Text("Project Name");
                static char projectName[128] = ""; // Buffer for project name input
                ImGui::InputText("##ProjectName", projectName, IM_ARRAYSIZE(projectName));
    
                ImGui::Spacing(); // Add spacing between elements

                ImGui::Text("Project Directory");

                // Display the read-only directory path with some padding for layout
                static char projectDir[256];
                ImGui::SameLine();
                ImGui::TextWrapped("%s", projectDir);

                // Add a button for selecting the directory path, positioned on the same line
                ImGui::SameLine();
                if (ImGui::Button("Choose..."))
                {
                    FileDialog.Open();
                    std::filesystem::path chosenPath = FileDialog.GetSelected();
                    if (!chosenPath.empty())
                    {
                        strncpy(projectDir, chosenPath.string().c_str(), sizeof(projectDir) - 1);
                    }
                }
                FileDialog.Display();

                ImGui::Spacing(); // Add spacing between elements
    
                // Add a separator for visual separation
                ImGui::Separator();
    
                // Centered buttons for 'Cancel' and 'Create'
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 220) * 0.5f);  // Center the buttons
    
                if (ImGui::Button("Cancel", ImVec2(100, 30)))
                {
                    PanelManager::GetPanel<NewProjectPanel>()->SetVisible(false);
                    ImGui::CloseCurrentPopup();  // Close the modal
                }
    
                ImGui::SameLine();  // Place the next button on the same line
    
                if (ImGui::Button("Create", ImVec2(100, 30)))
                {
                    // Handle create button click, create the project here
                    if (projectName[0] != '\0' && projectDir[0] != '\0')
                    {
                        Project::New(projectName, projectDir);  // Implement the function for project creation
                        PanelManager::GetPanel<NewProjectPanel>()->SetVisible(false);
                        ImGui::CloseCurrentPopup();  // Close the modal after creating
                    }
                }
                ImGui::EndPopup();  // End the modal popup
            }
        }
        ImGui::End();  // End the window
    }



    void NewProjectPanel::OnEvent(FEvent& InEvent)
    {
    }
}
