#include "pch.h"
#include "Log.h"

#include <filesystem>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Sinks/ConsoleSink.h"
#include "spdlog/sinks/ringbuffer_sink.h"

namespace Lumina
{

	std::shared_ptr<spdlog::logger> FLog::Logger;
	std::vector<ConsoleMessage> FLog::Logs;

	FLog::FLog()
	{
	}

	FLog::~FLog()
	{
	}

	void FLog::Init()
	{
		
		spdlog::set_pattern("%^[%T] %n: %v%$");
		Logger = spdlog::stdout_color_mt("Lumina");
		Logger->sinks().push_back(std::make_shared<ConsoleSink>(Logs));
		Logger->set_level(spdlog::level::trace);
	
		LOG_TRACE("------- Log Initialized -------");

	}

	std::shared_ptr<spdlog::sinks::sink> FLog::GetSink()
	{
		return Logger->sinks().front();
	}

	void FLog::Shutdown()
	{
		LOG_TRACE("------- Log Shutdown -------");
		spdlog::shutdown();
		Logger = nullptr;
	}
}