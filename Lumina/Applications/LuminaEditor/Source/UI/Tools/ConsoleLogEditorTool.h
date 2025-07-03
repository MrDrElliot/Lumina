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

        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void DrawLogWindow(const FUpdateContext& UpdateContext, bool bIsFocused);
        void ProcessCommand(const FString& Command);

    private:

        TVector<FConsoleMessage>    OutputMessages;
        TDeque<FString>             CommandHistory;
        FString                     CurrentCommand;
        uint64                      HistoryIndex;
        bool                        ScrollToBottom = false;

        bool bShowTrace = true;
        bool bShowDebug = true;
        bool bShowInfo  = true;
        bool bShowWarn  = true;
        bool bShowError = true;
        bool bShowCritical = true;
        bool bColorWholeRow = true;
    };
}