
#include "Sandbox.h"
#include "EntryPoint.h"

void FSandbox::CreateEngine()
{
	Engine = FMemory::New<FSandboxEngine>();
	Engine->Initialize(this);
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