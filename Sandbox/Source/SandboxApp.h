#pragma once
#include "SandboxLayer.h"
#include "Source/Runtime/Core/Application.h"


class FSandboxApp : public Lumina::FApplication
{
public:

	FSandboxApp(const Lumina::FApplicationSpecs& InAppSpecs) : FApplication(InAppSpecs)
	{
		PushLayer(new FSandboxLayer());
	}

	void OnUpdate() override;
	
};
