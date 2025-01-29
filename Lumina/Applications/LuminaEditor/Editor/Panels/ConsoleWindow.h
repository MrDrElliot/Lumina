#pragma once

#include <deque>

#include "ImGuiWindow.h"

namespace Lumina
{
    class ConsoleWindow : public EditorImGuiWindow
    {
    public:

        ConsoleWindow()
        {
            Name = "Console";
            bVisible = true;
            
            // Initialize with some commands
            CommandHistory.push_back("help");
            CommandHistory.push_back("clear");
            CommandHistory.push_back("exit");

            CurrentCommand.clear();
            HistoryIndex = -1;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
        void ProcessCommand(const std::string& command);

    private:

        std::vector<Lumina::ConsoleMessage> OutputMessages;    // Stores the log messages
        std::deque<std::string> CommandHistory;                // Stores the command history
        std::string CurrentCommand;                            // Stores the current input
        int HistoryIndex;                                      // Keeps track of the current command history index
        bool ScrollToBottom = false;                           // Flag to scroll to the bottom of the output
    
    };
}
