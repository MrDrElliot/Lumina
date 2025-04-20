#pragma once

#include "Core/Engine/Engine.h"
#include "Core/Object/Object.h"
#include "Source/Runtime/Core/Application/Application.h"
#include "Sandbox.generated.h"

using namespace Lumina;

LUM_CLASS()
class CTestClass : public CObject
{
public:

	GENERATED_BODY()

	
	LUM_FIELD()
	uint8 Value;

	LUM_FIELD()
	uint16 Value16;

	LUM_FIELD()
	uint32 Value32;
	
};

LUM_CLASS()
class COtherClass : public CObject
{
public:

	GENERATED_BODY()

	LUM_FIELD()
	uint8 Index;

	LUM_FIELD()
	int64 Int64Value;
};


class FSandboxEngine : public FEngine
{
public:

	void CreateDevelopmentTools() override { }

        
private:
	
};

using namespace Lumina;

class FSandbox : public Lumina::FApplication
{
public:

	FSandbox() :FApplication("Sandbox") {}

	void EngineLoopCallback(const Lumina::FUpdateContext& UpdateContext);
	void CreateEngine() override;
	bool ApplicationLoop() override;
	bool Initialize(int argc, char** argv) override;
	void Shutdown() override;


private:
	
};
