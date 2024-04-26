#include "ContentBrowserPanel.h"

#include "imgui_internal.h"
#include <filesystem>
namespace fs = std::filesystem;


namespace Lumina
{
    FContentBrowserPanel::FContentBrowserPanel()
    {
        Name = "Content Browser";

    }

    FContentBrowserPanel::~FContentBrowserPanel()
    {
        
    }

    void FContentBrowserPanel::OnAdded()
    {
        
    }

    void FContentBrowserPanel::OnRemoved()
    {
    }
    
    void FContentBrowserPanel::OnNewScene()
    {
    }

    void FContentBrowserPanel::OnRender(double DeltaTime)
    {
              ImGui::Begin(Name.c_str());


        float leftPaneWidth = 200.0f;
        float rightPaneWidth = ImGui::GetContentRegionAvail().x - leftPaneWidth;

        // Left pane for directory view
        ImGui::BeginChild("Directories", ImVec2(leftPaneWidth, 0), true);
        for (auto& p : fs::directory_iterator(CurrentPath))
        {
            if (p.is_directory())
            {
                if (ImGui::Selectable(p.path().filename().string().c_str(), SelectedDirectory == p.path()))
                {
                    SelectedDirectory = p.path().string();
                    // Update the content view for the selected directory
                }
            }
        }
        ImGui::EndChild();

  
        ImGui::SameLine();

        // Right pane for content view
        ImGui::BeginChild("Contents", ImVec2(rightPaneWidth, 0), true);
        if (!SelectedDirectory.empty())
        {
            float paneWidth = ImGui::GetContentRegionAvail().x;
            float buttonSize = 100.0f; // Assuming square buttons for simplicity
            float cellSize = buttonSize + ImGui::GetStyle().ItemSpacing.x; // Total size of cell including spacing
            int itemsPerRow = (std::max)(1, static_cast<int>(paneWidth / cellSize));

            int itemIndex = 0;
            for (auto& p : std::filesystem::directory_iterator(SelectedDirectory)) {
                if (itemIndex % itemsPerRow != 0) {
                    ImGui::SameLine();
                }

                ImGui::PushID(itemIndex);
                ImGui::BeginGroup(); // Start of button+text group

                // Button
                ImGui::Button("##Icon", ImVec2(buttonSize, buttonSize)); // Placeholder for Icon

                // Center text below the button
                float textWidth = ImGui::CalcTextSize(p.path().filename().string().c_str()).x;
                // Since we want the text to wrap within the button width, calculate text start position for centering
                float textStartPos = (buttonSize - (textWidth > buttonSize ? buttonSize : textWidth)) * 0.5f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textStartPos);
                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize); // Ensure text wraps within button width

                // Text under button
                ImGui::TextUnformatted(p.path().filename().string().c_str());

                ImGui::PopTextWrapPos();

                ImGui::EndGroup(); // End of button+text group
                ImGui::PopID();

                ++itemIndex;
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }
}
