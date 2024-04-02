#include <Lumina.h>
#include "EntryPoint.h"

#include "SandboxApp.h"

Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	return new FSandboxApp();
}