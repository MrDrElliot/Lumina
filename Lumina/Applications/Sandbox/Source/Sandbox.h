#pragma once

#include "Core/Engine/Engine.h"
#include "Core/Object/Object.h"
#include "Source/Runtime/Core/Application/Application.h"
#include "Core/Object/ObjectPtr.h"


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
