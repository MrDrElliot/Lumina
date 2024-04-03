#pragma once
#include <Lumina.h>


class FSandboxApp : public Lumina::FApplication
{
public:

	FSandboxApp(const Lumina::FApplicationSpecs& InAppSpecs) : FApplication(InAppSpecs)
	{
		std::cout << "SandboxApp";
		LE_LOG_CRITICAL("Critical");
		LE_LOG_ERROR("Error");
		LE_LOG_WARN("Warn");
		LE_LOG_TRACE("Trace");
		LE_LOG_INFO("Info");
	}
	
};