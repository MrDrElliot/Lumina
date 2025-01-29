#pragma once

#include "Core/Application/Application.h"

namespace Lumina
{
    class FCamera;
}

namespace Lumina
{
    class FEditorSettings;
    class FEditorLayer;
    class FEditorPanel;

    class LuminaEditor : public FApplication
    {
    public:
    
        LuminaEditor(const FApplicationSpecs& AppSpecs);

        void OnInit() override;

        void CreateProject();
        void OpenProject();

        void OnShutdown() override;

        void OnEvent(FEvent& Event) override;

        TRefPtr<FEditorLayer> GetEditorLayer() { return EditorLayer; }

    private:

        TRefPtr<FEditorLayer>       EditorLayer;
        TSharedPtr<FCamera>    EditorCamera;
        
    };
    
    inline LuminaEditor* GEditor;
}

