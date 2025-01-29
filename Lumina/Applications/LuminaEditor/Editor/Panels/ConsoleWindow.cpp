#include "ConsoleWindow.h"

#include "imgui.h"
#include "Log/Sinks/ConsoleSink.h"

namespace Lumina
{
    inline std::string LogLevelToString(EConsoleLogLevel level)
    {
        switch (level)
        {
        case EConsoleLogLevel::Trace:
            return "TRACE";
        case EConsoleLogLevel::Debug:
            return "DEBUG";
        case EConsoleLogLevel::Info:
            return "INFO";
        case EConsoleLogLevel::Warn:
            return "WARN";
        case EConsoleLogLevel::Error:
            return "ERROR";
        case EConsoleLogLevel::Off:
            return "OFF";
        default:
            return "UNKNOWN";
        }
    }

    void ConsoleWindow::OnAttach()
    {
    }

    void ConsoleWindow::OnDetach()
    {
    }

    void ConsoleWindow::OnUpdate(double DeltaTime)
    {
        // Start a new ImGui window
        ImGui::Begin("Console", nullptr, ImGuiWindowFlags_MenuBar);

        // Set a larger font size (you can adjust the value here)
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

        // Change the background color of the log output area (dark grey for better contrast)
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

        // Make the output area scrollable and improve the appearance
        ImGui::BeginChild("Output", ImVec2(0, -30), true, ImGuiWindowFlags_HorizontalScrollbar);
        {
            FLog::GetConsoleLogs(OutputMessages);  // This is a getter for the logs
            // Display all the log messages with full formatting (Time, Logger Name, Log Level, Message)
            for (const auto& message : OutputMessages)
            {
                // Format the log message using the log details
                FString formattedMessage = "[" + message.Time + "] [" + message.LoggerName + "]: " + message.Message;

                // Apply color based on log level
                if (message.Level == EConsoleLogLevel::Error)
                {
                    // Red for errors
                    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", formattedMessage.c_str());  // Red color for errors
                }
                else if (message.Level == EConsoleLogLevel::Warn)
                {
                    // Orange for warnings (warn messages usually imply caution)
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "%s", formattedMessage.c_str());  // Orange color for warnings
                }
                else if (message.Level == EConsoleLogLevel::Info)
                {
                    // Light gray for info messages
                    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "%s", formattedMessage.c_str());  // Light gray for info
                }
                else if (message.Level == EConsoleLogLevel::Debug)
                {
                    // Light blue for debug messages
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", formattedMessage.c_str());  // Light blue for debug
                }
                else if (message.Level == EConsoleLogLevel::Trace)
                {
                    // Dark gray for trace messages
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", formattedMessage.c_str());  // Dark gray for trace
                }
                else
                {
                    // Default light gray color for other messages
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", formattedMessage.c_str());
                }
            }

            // Ensure that the scroll is always at the bottom when new messages are added
            if (ScrollToBottom)
            {
                ImGui::SetScrollHereY(1.0f);  // Scroll to bottom
                ScrollToBottom = false;
            }
        }
        ImGui::EndChild();

        // Pop the background color and font size changes
        ImGui::PopStyleColor();
        ImGui::PopFont();

        // Input text area at the bottom
        ImGui::Separator();
        if (ImGui::InputText("Input", CurrentCommand.data(), CurrentCommand.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ProcessCommand(CurrentCommand);
            CommandHistory.push_back(CurrentCommand);  // Save command to history
            HistoryIndex = CommandHistory.size();     // Reset to the latest history entry
            CurrentCommand.clear();                   // Clear the current input
        }

        // Handle command history scrolling (up and down keys)
        if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            if (HistoryIndex > 0) 
            {
                HistoryIndex--;
                CurrentCommand = CommandHistory[HistoryIndex];
            }
        }
        if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            if (HistoryIndex < CommandHistory.size() - 1)
            {
                HistoryIndex++;
                CurrentCommand = CommandHistory[HistoryIndex];
            }
            else if (HistoryIndex == CommandHistory.size() - 1)
            {
                HistoryIndex++;
                CurrentCommand.clear();
            }
        }

        ImGui::End();
    }

    void ConsoleWindow::OnEvent(FEvent& InEvent)
    {
    }

    void ConsoleWindow::ProcessCommand(const std::string& command)
    {
        if (command.empty())
        {
            return;
        }
    
        // Add the command to history
        CommandHistory.push_back(command);
        HistoryIndex = CommandHistory.size();

        // Process the command (just an example of adding echo and clear functionality)
        if (command == "clear")
        {
            OutputMessages.clear();
        }
    }
}
