#include "SandboxApp.h"
#include "Source/Runtime/Core/Application.h"
#include "EntryPoint.h"


std::unique_ptr<Lumina::FApplication> Lumina::CreateApplication(int argc, char** argv)
{
	FApplicationSpecs AppSpecs;
	AppSpecs.Name = "Sandbox";
	AppSpecs.WindowWidth = 1600;
	AppSpecs.WindowHeight = 900;
	AppSpecs.bRenderImGui = false;
	
	return std::make_unique<FSandboxApp>(AppSpecs);

}
