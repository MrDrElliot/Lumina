#pragma once
#include "Source/Runtime/ApplicationCore/Application.h"
#include "EntryPoint.h"

namespace Lumina
{
    class FEditorLayer;
}

namespace Lumina
{
    class FEditorPanel;

    class LuminaEditor : public FApplication
    {
    public:
        LuminaEditor(const FApplicationSpecs& AppSpecs);
        ~LuminaEditor();

        void OnInit() override;
        void RenderImGui() override;

        
        
    };
    

}
