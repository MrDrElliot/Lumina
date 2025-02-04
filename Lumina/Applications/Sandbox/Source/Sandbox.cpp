
#include "Sandbox.h"
#include "EntryPoint.h"

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