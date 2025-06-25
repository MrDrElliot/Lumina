#pragma once
#include "EditorTool.h"

namespace Lumina
{
    class FConsoleLogEditorTool : public FEditorTool
    {
    public:

        LUMINA_SINGLETON_EDITOR_TOOL(FConsoleLogEditorTool)
    
        FConsoleLogEditorTool(IEditorToolContext* Context)
            : FEditorTool(Context, "Console", nullptr)
            , HistoryIndex(0)
        {}

        bool IsSingleWindowTool() const override { return true; }
        const char* GetTitlebarIcon() const override { return LE_ICON_FORMAT_LIST_BULLETED_TYPE; }
        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;
        
        void DrawLogWindow(const FUpdateContext& UpdateContext, bool bIsFocused);
        void ProcessCommand(const std::string& Command);

    private:

        std::vector<ConsoleMessage> OutputMessages;
        TDeque<std::string> CommandHistory;
        std::string CurrentCommand;
        uint64 HistoryIndex;
        bool ScrollToBottom = false;
    };
}