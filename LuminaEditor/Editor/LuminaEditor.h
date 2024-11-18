#pragma once

#include "Core/Application.h"


namespace Lumina
{
    class FEditorSettings;
}

namespace Lumina
{
    class FEditorLayer;
    class FEditorPanel;

    class LuminaEditor : public FApplication
    {
    public:
    
        LuminaEditor(const FApplicationSpecs& AppSpecs);
        ~LuminaEditor();

        void OnInit() override;
        void PostFrame() override;

        void CreateProject();
        void OpenProject();

        void OnShutdown() override;

        void OnEvent(FEvent& Event) override;

        TRefPtr<FEditorLayer> GetEditorLayer() { return EditorLayer; }

    private:

        TRefPtr<FEditorLayer> EditorLayer;
        
    };
    
    inline LuminaEditor* GEditor;
}

