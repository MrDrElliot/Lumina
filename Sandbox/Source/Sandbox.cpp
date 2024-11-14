#include <pch.h>

#include "SandboxApp.h"
#include "EntryPoint.h"


Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	FApplicationSpecs AppSpecs;
	AppSpecs.Name = "Sandbox";
	AppSpecs.WindowWidth = 1600;
	AppSpecs.WindowHeight = 900;
	return new FSandboxApp(AppSpecs);

}