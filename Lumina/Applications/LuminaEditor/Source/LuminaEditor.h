#pragma once

#include "Core/Application/Application.h"

namespace Lumina
{
    class FCamera;
    class FEditorSettings;
    class FEditorLayer;
    class FEditorPanel;

    inline class FEditorEngine* GEditorEngine = nullptr;
    
    class FEditorEngine : public FEngine
    {
    public:

        void CreateDevelopmentTools() override;
    
    private:
    };
    

    class LuminaEditor : public FApplication
    {
    public:
    
        LuminaEditor();

        bool Initialize(int argc, char** argv) override;
        void CreateEngine() override;
        
        bool ApplicationLoop() override;
        
        void CreateProject();
        void OpenProject();

        void RenderDeveloperTools(const FUpdateContext& UpdateContext) override;
        
        void Shutdown() override;
    
    private:
        
        
    };

    
    
}
