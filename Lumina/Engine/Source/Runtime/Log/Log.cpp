#include "Log.h"

#include <filesystem>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>

namespace Lumina
{

	std::shared_ptr<spdlog::logger> FLog::Logger;

	FLog::FLog()
	{
		std::cout << "HEllo";
	}

	FLog::~FLog()
	{
	}

	void FLog::Init()
	{
		
		std::string LogsDirectory = "Logs";

		spdlog::set_pattern("%^[%T] %n: %v%$");
		Logger = spdlog::stdout_color_mt("LogLumina");
		Logger->set_level(spdlog::level::trace);
	
		LE_LOG_TRACE	("------- Log Initialized -------");

	}
	void FLog::Shutdown()
	{
		LE_LOG_WARN("Logger: Shutting Down");
		spdlog::shutdown();
	}
}