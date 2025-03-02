
#include "Sandbox.h"
#include "EntryPoint.h"
#include "Input/Input.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"


void FSandbox::CreateEngine()
{
	Engine = FMemory::New<FSandboxEngine>();
	Engine->Initialize(this);
	Engine->SetUpdateCallback([this] (const Lumina::FUpdateContext& UpdateContext)
	{
		EngineLoopCallback(UpdateContext);
	});
}

void FSandbox::EngineLoopCallback(const Lumina::FUpdateContext& UpdateContext)
{
	Lumina::IRenderContext* RenderContext = UpdateContext.GetSubsystem<Lumina::FRenderManager>()->GetRenderContext();
	RenderContext->GetCommandList()->ClearColor(Lumina::FColor::Red);
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