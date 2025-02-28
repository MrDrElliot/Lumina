#include "LuminaEditor.h"
#include "EntryPoint.h"
#include "Core/Threading/Thread.h"
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
        //Update LUNINA_DIR (engine directory) every-time the editor is ran.
        //Paths::SetEnvVariable("LUMINA_DIR", std::filesystem::current_path().parent_path().string());
        LOG_DEBUG("LUMA_DIR Path: {0}", std::getenv("LUMINA_DIR"));
        LOG_DEBUG("Current Working Path: {0}",  std::filesystem::current_path().string());
        LOG_DEBUG("Current Engine Install Path: {0}", Paths::GetEngineInstallDirectory().string());
        LOG_DEBUG("Current Engine Directory: {0}", Paths::GetEngineDirectory().string());
        LOG_DEBUG("Number of Threads: {0}", Threading::GetNumThreads());

        return true;
    }

    void LuminaEditor::CreateEngine()
    {
        Engine = new FEditorEngine();
        Engine->SetUpdateCallback([] { });
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
