#pragma once

#include "Core/Application/Application.h"

namespace Lumina
{
    class FCamera;
    class FEditorSettings;
    class FEditorLayer;
    class FEditorPanel;

    class FEditorEngine : public FEngine
    {
    public:


    private:
    };
    

    class LuminaEditor : public FApplication
    {
    public:
    
        LuminaEditor();

        bool Initialize() override;
        bool ApplicationLoop() override;
        
        void CreateProject();
        void OpenProject();

        void CreateImGuiPanels();
        
        void Shutdown() override;
        void OnEvent(FEvent& Event) override;
    
    private:
        
        TSharedPtr<FCamera>    EditorCamera;
        
    };
    
}
