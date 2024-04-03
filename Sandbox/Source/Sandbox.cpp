#include <Lumina.h>
#include "EntryPoint.h"

#include "SandboxApp.h"

Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	FApplicationSpecs AppSpecs;
	AppSpecs.Name = "Sandbox";
	AppSpecs.WindowHeight = 1900;
	AppSpecs.WindowWidth = 800;

	return new FSandboxApp(AppSpecs);
}