#pragma once

#include "Source/Runtime/Core/Application/Application.h"


class FSandbox : public Lumina::FApplication
{
public:

	FSandbox() :FApplication("Sandbox") {}

	void CreateEngine() override { }
	bool ApplicationLoop() override;
	bool Initialize() override;
	void Shutdown() override;


private:
	
};
