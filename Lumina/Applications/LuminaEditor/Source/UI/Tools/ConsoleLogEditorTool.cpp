#include "ConsoleLogEditorTool.h"

namespace Lumina
{
    void FConsoleLogEditorTool::OnInitialize()
    {
        CreateToolWindow("Console", [this] (const FUpdateContext& UpdateContext, bool bIsFocused)
        {
           DrawLogWindow(UpdateContext, bIsFocused); 
        });
    }

    void FConsoleLogEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
        
    }

    void FConsoleLogEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::BeginMenu(LE_ICON_FILTER " Filter"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 2));

            ImGui::Checkbox("Trace",    &bShowTrace);
            ImGui::Checkbox("Debug",    &bShowDebug);
            ImGui::Checkbox("Info",     &bShowInfo);
            ImGui::Checkbox("Warning",  &bShowWarn);
            ImGui::Checkbox("Error",    &bShowError);
            ImGui::Checkbox("Critical", &bShowCritical);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("All"))
            {
                bShowTrace = bShowDebug = bShowInfo = bShowWarn = bShowError = bShowCritical = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("None"))
            {
                bShowTrace = bShowDebug = bShowInfo = bShowWarn = bShowError = bShowCritical = false;
            }
            ImGui::PopStyleVar(2);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu(LE_ICON_COG " Settings"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 2));
            
            ImGui::Checkbox("Color Whole Row", &bColorWholeRow);
            ImGui::Checkbox("Auto Scroll", &bAutoScroll);

            ImGui::PopStyleVar(2);

            ImGui::EndMenu();
        }
    }

    void FConsoleLogEditorTool::DrawLogWindow(const FUpdateContext& UpdateContext, bool bIsFocused)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

        ImGui::BeginChild("LogMessages", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
        
        const TFixedVector<FConsoleMessage, 10000>& Messages = Logging::GetConsoleLogs();
        SIZE_T NewMessageSize = Messages.size();
        
        if (NewMessageSize > PreviousMessageSize)
        {
            ScrollToBottom = true;
        }

        PreviousMessageSize = NewMessageSize;
        
        if (ImGui::BeginTable("LogTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX))
        {
            ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Logger", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            ImGuiListClipper Clipper;
            Clipper.Begin((int)NewMessageSize);

            while (Clipper.Step())
            {
                for (int i = Clipper.DisplayStart; i < Clipper.DisplayEnd; ++i)
                {
                    const FConsoleMessage& Message = Messages[i];
        
                    // Filtering logic
                    switch (Message.Level)
                    {
                    case spdlog::level::trace:    if (!bShowTrace) continue; break;
                    case spdlog::level::debug:    if (!bShowDebug) continue; break;
                    case spdlog::level::info:     if (!bShowInfo)  continue; break;
                    case spdlog::level::warn:     if (!bShowWarn)  continue; break;
                    case spdlog::level::err:      if (!bShowError) continue; break;
                    case spdlog::level::critical: if (!bShowCritical) continue; break;
                    default: break;
                    }
        
                        
                    ImVec4 Color;
                    const char* LevelLabel = "";
                    switch (Message.Level)
                    {
                        case spdlog::level::level_enum::err:   Color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); LevelLabel = LE_ICON_INFORMATION " Error"; break;
                        case spdlog::level::level_enum::warn:  Color = ImVec4(1.0f, 0.6f, 0.0f, 1.0f); LevelLabel = LE_ICON_INFORMATION " Warn";  break;
                        case spdlog::level::level_enum::info:  Color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); LevelLabel = LE_ICON_INFORMATION " Info";  break;
                        case spdlog::level::level_enum::debug: Color = ImVec4(0.4f, 0.8f, 1.0f, 1.0f); LevelLabel = LE_ICON_INFORMATION " Debug"; break;
                        case spdlog::level::level_enum::trace: Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); LevelLabel = LE_ICON_INFORMATION " Trace"; break;
                    }
                        
                    ImGui::TableNextRow();
                        
                    // Level
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextColored(Color, "%s", LevelLabel);
                        
                    // Time
                    ImGui::TableSetColumnIndex(1);
                    if (bColorWholeRow)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, Color);
                        ImGui::TextWrapped("%s", Message.Time.c_str());
                        ImGui::PopStyleColor();
                    }
                    else
                    {
                        ImGui::TextWrapped("%s", Message.Time.c_str());
                    }
                        
                    // Logger Name
                    ImGui::TableSetColumnIndex(2);
                    if (bColorWholeRow)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, Color);
                        ImGui::TextWrapped("%s", Message.LoggerName.c_str());
                        ImGui::PopStyleColor();
                    }
                    else
                    {
                        ImGui::TextWrapped("%s", Message.LoggerName.c_str());
                    }
                    
                    // Message
                    ImGui::TableSetColumnIndex(3);
                    if (bColorWholeRow)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, Color);
                        ImGui::TextWrapped("%s", Message.Message.c_str());
                        ImGui::PopStyleColor();
                    }
                    else
                    {
                        ImGui::TextWrapped("%s", Message.Message.c_str());
                    }
                }
            }

            if (ScrollToBottom && bAutoScroll)
            {
                ImGui::SetScrollHereY(0.999f);
                ScrollToBottom = false;
            }
            
            ImGui::EndTable();
        }
    
        ImGui::EndChild();
        ImGui::PopStyleColor();
    
        ImGui::Separator();
        ImGui::SetNextItemWidth(-1);
        
        char InputBuffer[512];
        strncpy(InputBuffer, CurrentCommand.c_str(), sizeof(InputBuffer));
        InputBuffer[sizeof(InputBuffer) - 1] = '\0';

        if (ImGui::InputText("##Input", InputBuffer, sizeof(InputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            CurrentCommand = InputBuffer;
            LOG_INFO("Command: {}", CurrentCommand);
            ProcessCommand(CurrentCommand);
            CommandHistory.push_back(CurrentCommand);
            HistoryIndex = CommandHistory.size();
            CurrentCommand.clear();
            ScrollToBottom = true;
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



    void FConsoleLogEditorTool::ProcessCommand(const FString& Command)
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
