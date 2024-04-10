#include "MainMenuPanel.h"

#include "imgui.h"


namespace Lumina
{
    FMainMenuPanel::FMainMenuPanel()
    {
        Name = "Main Menu";
    }

    FMainMenuPanel::~FMainMenuPanel()
    {
    }

    void FMainMenuPanel::OnAdded()
    {
    }

    void FMainMenuPanel::OnRemoved()
    {
    }

    void FMainMenuPanel::OnRender()
    {
    }

    void FMainMenuPanel::OnImGui()
    {
        
        if (ImGui::BeginMainMenuBar())
        {
    
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {
                    
                }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {

                }
                if (ImGui::MenuItem("Exit", "Ctrl+Q"))
                {

                }
        
                ImGui::EndMenu();
            }
    
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z"))
                {
                    
                }
        
                ImGui::EndMenu();
            }
    
            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }

        
    }

    void FMainMenuPanel::OnNewScene()
    {
    }
}
