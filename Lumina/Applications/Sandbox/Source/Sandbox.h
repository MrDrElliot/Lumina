#pragma once

#include "Source/Runtime/Core/Application/Application.h"
#include "Core/Engine/Engine.h"

namespace Lumina
{
	class FScene;
}

class FSandboxEngine : public Lumina::FEngine
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
	bool Initialize() override;
	void Shutdown() override;


	FScene* Scene;
	
private:
	
};
