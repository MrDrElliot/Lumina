#include "pch.h"
#include "Log.h"

#include <filesystem>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Sinks/ConsoleSink.h"
#include "spdlog/sinks/ringbuffer_sink.h"

namespace Lumina::Logging
{
	
	LUMINA_API std::shared_ptr<spdlog::logger> Logger;
	LUMINA_API TFixedVector<FConsoleMessage, 10000> Logs;
	
	bool IsInitialized()
	{
		return Logger != nullptr;
	}

	void Init()
	{
		Logs.reserve(10000);
		spdlog::set_pattern("%^[%T] %n: %v%$");
		Logger = spdlog::stdout_color_mt("Lumina");
		Logger->sinks().push_back(std::make_shared<ConsoleSink>(Logs));
		Logger->set_level(spdlog::level::trace);
	
		LOG_TRACE("------- Log Initialized -------");

	}

	std::shared_ptr<spdlog::sinks::sink> GetSink()
	{
		return Logger->sinks().front();
	}

	void Shutdown()
	{
		LOG_TRACE("------- Log Shutdown -------");
		spdlog::shutdown();
		Logger = nullptr;
	}

	std::shared_ptr<spdlog::logger> GetLogger()
	{
		return Logger;
	}

	const TFixedVector<FConsoleMessage, 10000>& GetConsoleLogs()
	{
		return Logs;
	}
}
