
#include "Sandbox.h"
#include "EntryPoint.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Math/Transform.h"
#include "Input/Input.h"
#include "Scene/Entity/Systems/EntitySystem.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Scene/Subsystems/FCameraManager.h"

using namespace Lumina;


void FSandbox::CreateEngine()
{
	Engine = FMemory::New<FSandboxEngine>();
	Engine->Initialize(this);
	Engine->SetUpdateCallback([this] (const FUpdateContext& UpdateContext)
	{
		EngineLoopCallback(UpdateContext);
	});
}

void FSandbox::EngineLoopCallback(const FUpdateContext& UpdateContext)
{
	
}

bool FSandbox::ApplicationLoop()
{
	return true;
}

bool FSandbox::Initialize()
{
	return true;
}

void FSandbox::Shutdown()
{
}


Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	return new FSandbox();
}