#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>


namespace Lumina
{
	class FLog
	{
	public:

		FLog();
		~FLog();

		static void Init();
		static void Shutdown();


		inline static std::shared_ptr<spdlog::logger> GetLogger() { return Logger; }

	private:

		static std::shared_ptr<spdlog::logger> Logger;
	};
}

/* Core Logging Macros */
#define LE_LOG_CRITICAL(...) ::Lumina::FLog::GetLogger()->critical(__VA_ARGS__)
#define LE_LOG_ERROR(...) ::Lumina::FLog::GetLogger()->error(__VA_ARGS__)
#define LE_LOG_WARN(...) ::Lumina::FLog::GetLogger()->warn(__VA_ARGS__)
#define LE_LOG_TRACE(...) ::Lumina::FLog::GetLogger()->trace(__VA_ARGS__)
#define LE_LOG_INFO(...) ::Lumina::FLog::GetLogger()->info(__VA_ARGS__)
