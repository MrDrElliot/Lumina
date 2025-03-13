#include "LuminaEditor.h"
#include "EntryPoint.h"
#include "Core/Threading/Thread.h"
#include "Project/Project.h"
#include "Renderer/RenderResource.h"
#include "Scene/Scene.h"
#include "Settings/EditorSettings.h"
#include "UI/EditorUI.h"

namespace Lumina
{

    void FEditorEngine::CreateDevelopmentTools()
    {
        DeveloperToolUI = new FEditorUI();
    }

    
    FApplication* CreateApplication(int argc, char** argv)
    {
        return new LuminaEditor();
    }
    
    LuminaEditor::LuminaEditor()
        : FApplication("Lumina Editor", 1 << 0)
    {
    }
    
    bool LuminaEditor::Initialize()
    {

        FEditorSettings::Get()->LoadSettings();
        std::filesystem::path StartupProject = FEditorSettings::Get()->GetStartupProject().c_str();
        FProject::Get()->LoadProject(StartupProject.string().c_str());
        
        return true;
    }

    void LuminaEditor::CreateEngine()
    {
        Engine = new FEditorEngine();
        Engine->SetUpdateCallback([] (const FUpdateContext&) { });
        Engine->Initialize(this);
    }

    bool LuminaEditor::ApplicationLoop()
    {
        return true;
    }

    void LuminaEditor::CreateProject()
    {
        
    }

    void LuminaEditor::OpenProject()
    {
        
    }

    void LuminaEditor::RenderDeveloperTools(const FUpdateContext& UpdateContext)
    {
    }

    void LuminaEditor::Shutdown()
    {
        
    }
}
