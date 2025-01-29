#include "ToolbarWindow.h"

#include "EditorLayer.h"
#include "EditorSettingsPanel.h"
#include "imgui.h"
#include "LuminaEditor.h"
#include "Project/NewProjectPanel.h"
#include "Project/OpenProjectPanel.h"
#include "Project/PanelManager.h"
#include "Project/Project.h"

namespace Lumina
{
    void ToolbarWindow::OnAttach()
    {
    }

    void ToolbarWindow::OnDetach()
    {
    }

    void ToolbarWindow::OnUpdate(double DeltaTime)
    {
        // Create the main menu bar
        if (ImGui::BeginMainMenuBar())
        {
            // File Menu
            if (ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Editor Settings"))
                {
                    PanelManager::Get()->GetPanel<FEditorSettingsPanel>()->SetVisible(true);
                }
                if (ImGui::MenuItem("New Project"))
                {
                    PanelManager::Get()->GetPanel<NewProjectPanel>()->SetVisible(true);
                }
                if (ImGui::MenuItem("Open Project"))
                {
                    PanelManager::Get()->GetPanel<OpenProjectPanel>()->SetVisible(true);
                }
                if(ImGui::MenuItem("Project Settings"))
                {
                    bShowProjectSettings = !bShowProjectSettings;
                }
                if (ImGui::MenuItem("Save"))
                {
                    // Handle "Save" action
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit"))
                {
                    // Handle "Exit" action (close the program)
                }
                ImGui::EndMenu();
            }

            // Edit Menu
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo"))
                {
                    // Handle "Undo" action
                }
                if (ImGui::MenuItem("Redo"))
                {
                    // Handle "Redo" action
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Preferences"))
                {
                    // Handle "Preferences" action
                }
                ImGui::EndMenu();
            }

            // Windows Menu
            if (ImGui::BeginMenu("Windows"))
            {
                TArray<TRefPtr<EditorImGuiWindow>> Windows;
                PanelManager::Get()->GetPanels(Windows);
             
                // Loop through each window and create a menu item for it
                for (auto& Window : Windows)
                {
                    if(!Window->ShouldShowInWindows())
                    {
                        continue;
                    }
                    
                    // Use the window name for the menu item
                    FString menuItemName = Window->GetName();

                    // Check visibility and create the menu item to toggle the window's visibility
                    if (ImGui::MenuItem(menuItemName.CStr(), nullptr, Window->IsVisible()))
                    {
                        // Toggle the visibility of the window
                        Window->SetVisible(!Window->IsVisible());
                    }
                }
                if(ImGui::MenuItem("Show/Hide DearImGui Demo", nullptr, bShowImGuiDemo))
                {
                    bShowImGuiDemo = !bShowImGuiDemo;
                }
                ImGui::EndMenu();
            }

            if(bShowImGuiDemo)
            {
                ImGui::ShowDemoWindow();
            }

            if (bShowProjectSettings)
            {
                bool bOpen = true;
                ImGui::Begin("Project Settings", &bOpen, ImGuiWindowFlags_AlwaysAutoResize);

                // Project Name
                static char projectNameBuffer[256];
                strncpy(projectNameBuffer, Project::GetProjectConfig().Name.c_str(), sizeof(projectNameBuffer));
                if (ImGui::InputText("Project Name", projectNameBuffer, sizeof(projectNameBuffer)))
                {
                    Project::GetProjectConfig().Name = std::string(projectNameBuffer); // Update config when text is edited
                }

                // Project Version
                static char projectVersionBuffer[32];
                strncpy(projectVersionBuffer, Project::GetProjectConfig().ProjectVersion.c_str(), sizeof(projectVersionBuffer));
                if (ImGui::InputText("Project Version", projectVersionBuffer, sizeof(projectVersionBuffer)))
                {
                    Project::GetProjectConfig().ProjectVersion = std::string(projectVersionBuffer); // Update project version
                }

                // Engine Version (view only)
                ImGui::Text("Engine Version: %s", Project::GetProjectConfig().EngineVersion.c_str());
            
                ImGui::End();

                if(!bOpen)
                {
                    Project::GetCurrent()->Serialize();
                    bShowProjectSettings = false;
                }
            }

            ImGui::EndMainMenuBar();
        }

    }


    void ToolbarWindow::OnEvent(FEvent& InEvent)
    {
    }
}
