#include "ConsoleLogEditorTool.h"

namespace Lumina
{
    inline std::string LogLevelToString(EConsoleLogLevel level)
    {
        switch (level)
        {
            case EConsoleLogLevel::Trace: return "TRACE";
            case EConsoleLogLevel::Debug: return "DEBUG";
            case EConsoleLogLevel::Info: return "INFO";
            case EConsoleLogLevel::Warn: return "WARN";
            case EConsoleLogLevel::Error: return "ERROR";
            case EConsoleLogLevel::Off: return "OFF";
            default: return "UNKNOWN";
        }
    }
    
    void FConsoleLogEditorTool::OnInitialize(const FUpdateContext& UpdateContext)
    {
        CreateToolWindow("Console", [this] (const FUpdateContext& UpdateContext, bool bIsFocused)
        {
           DrawLogWindow(UpdateContext, bIsFocused); 
        });
    }

    void FConsoleLogEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
        
    }

    void FConsoleLogEditorTool::DrawLogWindow(const FUpdateContext& UpdateContext, bool bIsFocused)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        
            FLog::GetConsoleLogs(OutputMessages);
            for (const auto& message : OutputMessages)
            {
                FString formattedMessage = "[" + message.Time + "] [" + message.LoggerName + "]: " + message.Message;

                if (message.Level == EConsoleLogLevel::Error)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", formattedMessage.c_str());
                }
                else if (message.Level == EConsoleLogLevel::Warn)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "%s", formattedMessage.c_str());
                }
                else if (message.Level == EConsoleLogLevel::Info)
                {
                    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "%s", formattedMessage.c_str());
                }
                else if (message.Level == EConsoleLogLevel::Debug)
                {
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", formattedMessage.c_str());
                }
                else if (message.Level == EConsoleLogLevel::Trace)
                {
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", formattedMessage.c_str());
                }
                else
                {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", formattedMessage.c_str());
                }
            }

            if (ScrollToBottom)
            {
                ImGui::SetScrollHereY(1.0f);
                ScrollToBottom = false;
            }

        ImGui::PopStyleColor();

        ImGui::Separator();
        
        if (ImGui::InputText("Input", CurrentCommand.data(), CurrentCommand.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ProcessCommand(CurrentCommand);
            CommandHistory.push_back(CurrentCommand);
            HistoryIndex = CommandHistory.size();
            CurrentCommand.clear();
        }

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
    }

    void FConsoleLogEditorTool::ProcessCommand(const std::string& Command)
    {
        if (Command.empty())
        {
            return;
        }
    
        // Add the command to history
        CommandHistory.emplace_back(Command);
        HistoryIndex = CommandHistory.size();
    }
}
