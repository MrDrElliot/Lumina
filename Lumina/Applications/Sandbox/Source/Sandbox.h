#pragma once

#include "Source/Runtime/Core/Application/Application.h"
#include "Core/Engine/Engine.h"

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

	void CreateEngine() override;
	bool ApplicationLoop() override;
	bool Initialize() override;
	void Shutdown() override;


private:
	
};
