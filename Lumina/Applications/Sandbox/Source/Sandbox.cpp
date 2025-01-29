#include "SandboxApp.h"
#include "Source/Runtime/Core/Application.h"
#include "EntryPoint.h"


Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	FApplicationSpecs AppSpecs;
	AppSpecs.Name = "Sandbox";
	AppSpecs.bRenderImGui = false;
	
	return new FSandboxApp(AppSpecs);

}
