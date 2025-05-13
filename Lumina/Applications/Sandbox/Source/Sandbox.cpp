
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
	COtherClass* TestClass2 = NewObject<COtherClass>();
	COtherClass* TestClass3 = NewObject<COtherClass>();
	COtherClass* TestClass4 = NewObject<COtherClass>();
	COtherClass* TestClass5 = NewObject<COtherClass>();
	COtherClass* TestClass6 = NewObject<COtherClass>();
	COtherClass* TestClass7 = NewObject<COtherClass>();
	

	CClass* Class = TestClass->GetClass();
	FProperty* Prop = Class->GetProperty(GET_MEMBER_NAME_CHECKED(COtherClass, Value));
	Prop->SetValuePtr<uint8>(TestClass, 69);

	CEnum* Enum = StaticEnum<ETestEnum>();

	const TCHAR* Package = Class->GetPackage();
	
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
