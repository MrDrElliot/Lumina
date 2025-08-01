#include "LuminaEditor.h"
#include "EntryPoint.h"
#include "Project/Project.h"
#include "Lumina_eastl.cpp"
#include "Renderer/RenderResource.h"
#include "Scene/Scene.h"
#include "UI/EditorUI.h"

namespace Lumina
{
    void FEditorEngine::CreateDevelopmentTools()
    {
        DeveloperToolUI = Memory::New<FEditorUI>();
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
        Engine->Initialize(this);
        
        return true;
    }

    void LuminaEditor::CreateEngine()
    {
        GEditorEngine = Memory::New<FEditorEngine>();
        Engine = GEditorEngine;
        Engine->SetUpdateCallback([] (const FUpdateContext&) { });
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

