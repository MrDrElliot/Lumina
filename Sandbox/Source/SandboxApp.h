#pragma once
#include <Lumina.h>

class FSandboxApp : public Lumina::FApplication
{
public:

	FSandboxApp()
	{
		std::cout << "Hello World!";
	}
	
};