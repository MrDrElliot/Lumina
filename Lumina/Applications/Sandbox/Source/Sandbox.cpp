
#include "Sandbox.h"
#include "Lumina_eastl.cpp"
#include "EntryPoint.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Module/ModuleManager.h"
#include "Core/Object/Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Input/Input.h"
#include "Scene/Entity/Systems/EntitySystem.h"
#include "Renderer/RenderContext.h"

using namespace Lumina;


void FSandbox::CreateEngine()
{
	Engine = FMemory::New<FSandboxEngine>();
	Engine->Initialize(this);
	Engine->SetUpdateCallback(std::bind(&FSandbox::EngineLoopCallback, this, std::placeholders::_1));
}

void FSandbox::EngineLoopCallback(const FUpdateContext& UpdateContext)
{
}

bool FSandbox::ApplicationLoop()
{
	return true;
}

bool FSandbox::Initialize(int argc, char** argv)
{
	COtherClass* TestClass = NewObject<COtherClass>();

	FString Path = TestClass->GetPathName();
	
	return true;
}

void FSandbox::Shutdown()
{
}

Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	return new FSandbox();
}

namespace eastl 
{
	void AssertionFailure(const char* pExpression)
	{
		std::fprintf(stderr, "EASTL Assertion Failed: %s\n", pExpression);
		std::abort();
	}
}

DECLARE_MODULE_ALLOCATOR_OVERRIDES()
