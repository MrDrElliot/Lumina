#include "LuminaEditor.h"
#include "EntryPoint.h"
#include "Lumina_eastl.cpp"
#include "Core/Module/ModuleManager.h"
#include "Renderer/RenderResource.h"
#include "UI/EditorUI.h"

namespace Lumina
{
    bool FEditorEngine::Init(FApplication* App)
    {
        bool bSuccess = FEngine::Init(App);

        entt::locator<entt::meta_ctx>::reset(GetEngineMetaContext());

        return bSuccess;
    }

    IDevelopmentToolUI* FEditorEngine::CreateDevelopmentTools()
    {
        return Memory::New<FEditorUI>();
    }
    
    FApplication* CreateApplication(int argc, char** argv)
    {
        return Memory::New<LuminaEditor>();
    }
    
    LuminaEditor::LuminaEditor()
        : FApplication("Lumina Editor", 1 << 0)
    {
    }
    
    bool LuminaEditor::Initialize(int argc, char** argv)
    {
        GEngine->Init(this);
        
        return true;
    }

    FEngine* LuminaEditor::CreateEngine()
    {
        GEditorEngine = Memory::New<FEditorEngine>();
        GEngine = GEditorEngine;
        return GEngine;
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


DECLARE_MODULE_ALLOCATOR_OVERRIDES()

