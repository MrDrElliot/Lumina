#pragma once

#include <spdlog/spdlog.h>

#include "Memory/SmartPtr.h"
#include "Sinks/ConsoleSink.h"


namespace Lumina
{
	class FLog
	{
	public:

		FLog();
		~FLog();

		static bool IsInitialized() { return Logger != nullptr; }
		static void Init();
		static std::shared_ptr<spdlog::sinks::sink> GetSink();
		static void GetConsoleLogs(std::vector<ConsoleMessage>& OutLogs) { OutLogs = Logs; }
		static void Shutdown();


		inline static std::shared_ptr<spdlog::logger> GetLogger() { return Logger; }

	private:

		static std::shared_ptr<spdlog::logger> Logger;
		static std::vector<ConsoleMessage> Logs;
	};
}

/* Core Logging Macros */

#define LOG_CRITICAL(...) ::Lumina::FLog::GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) ::Lumina::FLog::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Lumina::FLog::GetLogger()->warn(__VA_ARGS__)
#define LOG_TRACE(...) ::Lumina::FLog::GetLogger()->trace(__VA_ARGS__)
#ifdef LE_DEBUG
#define LOG_DEBUG(...) ::Lumina::FLog::GetLogger()->debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...) // Empty definition; does nothing if LE_DEBUG is not defined
#endif
#define LOG_INFO(...) ::Lumina::FLog::GetLogger()->info(__VA_ARGS__)
