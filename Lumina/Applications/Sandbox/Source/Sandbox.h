#pragma once

#include "Core/Engine/Engine.h"
#include "Core/Object/Object.h"
#include "Source/Runtime/Core/Application/Application.h"
#include "Core/Object/ObjectPtr.h"
#include "Sandbox.generated.h"


LUM_ENUM()
enum class ETestEnum : uint8
{
	One = 14,
	Two = 200,
	Three = 222,
};

LUM_CLASS()
class CTestClass : public Lumina::CObject
{
	GENERATED_BODY()
	
public:

	CTestClass()
	{}
	
	LUM_PROPERTY()
	uint8 Value;

	LUM_PROPERTY()
	uint16 Value16;

	LUM_PROPERTY()
	uint32 Value32;

	LUM_PROPERTY()
	Lumina::TObjectPtr<CTestClass> TestObject;
};

LUM_CLASS()
class COtherClass : public CTestClass
{
	GENERATED_BODY()
	
public:

	LUM_PROPERTY()
	uint8 Index;

	LUM_PROPERTY()
	int64 Int64Value;
};


class FSandboxEngine : public Lumina::FEngine
{
public:

	void CreateDevelopmentTools() override { }

        
private:
	
};


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
