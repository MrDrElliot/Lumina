#pragma once

#include "LogMessage.h"
#include "Containers/Array.h"
#include "Core/DisableAllWarnings.h"


PRAGMA_DISABLE_ALL_WARNINGS
#include <spdlog/spdlog.h>
PRAGMA_ENABLE_ALL_WARNINGS

#include "Module/API.h"


namespace Lumina::Logging
{
	LUMINA_API bool IsInitialized();
	LUMINA_API void Init();
	LUMINA_API void Shutdown();
	

	/** DO NOT COPY THIS ARRAY. It can become very large and will become incredibly expensive */
	LUMINA_API const TVector<FConsoleMessage>& GetConsoleLogs();
	LUMINA_API std::shared_ptr<spdlog::logger> GetLogger();
	LUMINA_API std::shared_ptr<spdlog::sinks::sink> GetSink();
	

	
}

/* Core Logging Macros */

#define LOG_CRITICAL(...) ::Lumina::Logging::GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) ::Lumina::Logging::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Lumina::Logging::GetLogger()->warn(__VA_ARGS__)
#define LOG_TRACE(...) ::Lumina::Logging::GetLogger()->trace(__VA_ARGS__)
#ifdef LE_DEBUG
#define LOG_DEBUG(...) ::Lumina::Logging::GetLogger()->debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...) // Empty definition; does nothing if LE_DEBUG is not defined
#endif
#define LOG_INFO(...) ::Lumina::Logging::GetLogger()->info(__VA_ARGS__)
#define LOG_INFO_TAGGED(Tag, ...) ::Lumina::Logging::GetLogger()->info("[{}] {}", Tag, __VA_ARGS__)
