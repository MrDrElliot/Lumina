#include "PerformanceTrackerPanel.h"

#include "imgui.h"
#include "Containers/Array.h"
#include "Core/Performance/PerformanceTracker.h"

namespace Lumina
{
    void FPerformanceTrackerPanel::OnAttach()
    {
    }

    void FPerformanceTrackerPanel::OnDetach()
    {
        
    }

    void FPerformanceTrackerPanel::OnUpdate(double DeltaTime)
    {
        /*FPerformanceTracker* PerfTracker = FPerformanceTracker::Get();
        std::lock_guard<std::mutex> lock(PerfTracker->ProfileMapMutex);

        if (ImGui::Begin("Performance Profiling"))
        {
            ImGui::Text("Recent Profiling Results:");
        
            // Create a table with two columns: "Profile Name" and "Execution Time"
            if (ImGui::BeginTable("PerformanceTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoSavedSettings))
            {
                // Set table headers
                ImGui::TableSetupColumn("Profile Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Execution Time (ms)", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                // Loop through the profile map and sort by execution time (highest first)
                TVector<eastl::pair<FString, double>> sortedEntries(PerfTracker->ProfileMap.begin(), PerfTracker->ProfileMap.end());

                // Sort the vector by execution time in descending order
                std::sort(sortedEntries.begin(), sortedEntries.end(), [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b)
                {
                        return a.second > b.second; // Sort by time (highest first)
                });

                // Now loop through the sorted entries and display each one in the table
                for (const auto& entry : sortedEntries)
                {
                    // Set row background color based on time duration (optional)
                    if (entry.second > 10.0f) // Example: highlight long executions (e.g., > 10ms)
                        ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.6f, 0.0f, 0.0f, 0.3f)); // Red
                    else
                        ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Default

                    // New row for profile data
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", entry.first.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%.3f", entry.second); // Show the time in milliseconds

                    // Pop color style after each row
                    ImGui::PopStyleColor();
                }

                ImGui::EndTable();
            }

            ImGui::End();
        }*/
    }


    void FPerformanceTrackerPanel::OnEvent(FEvent& InEvent)
    {
    }
}
