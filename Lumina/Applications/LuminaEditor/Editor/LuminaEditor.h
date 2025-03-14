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

        void CreateDevelopmentTools() override;

        
    private:
    };
    
    inline FEditorEngine* GEditor = nullptr;

    class LuminaEditor : public FApplication
    {
    public:
    
        LuminaEditor();

        bool Initialize() override;
        void CreateEngine() override;
        
        bool ApplicationLoop() override;
        
        void CreateProject();
        void OpenProject();

        void RenderDeveloperTools(const FUpdateContext& UpdateContext) override;
        
        void Shutdown() override;
    
    private:
        
        
    };
    
}
