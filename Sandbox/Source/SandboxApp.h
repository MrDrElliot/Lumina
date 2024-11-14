#pragma once
#include <pch.h>

#include "SandboxLayer.h"


class FSandboxApp : public Lumina::FApplication
{
public:

	FSandboxApp(const Lumina::FApplicationSpecs& InAppSpecs) : FApplication(InAppSpecs)
	{
		PushLayer(new FSandboxLayer());
	}
	
};