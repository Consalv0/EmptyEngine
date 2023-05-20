#pragma once

#ifdef EMPTYENGINE_CORE
#pragma message ( "You have placed Core/Log.h before Core.h. This may cause errors in ftm headers" )
#else 
#define EMPTYENGINE_CORE_LOG
#endif // EMPTYENGINE_CORE

#include "Core.h"
// #define SPDLOG_WCHAR_TO_UTF8_SUPPORT
// #define SPDLOG_NO_DATETIME
// #include <spdlog/spdlog.h>
// #include <spdlog/fmt/ostr.h>

namespace EEngine 
{
	// class Log {
	// public:
	// 	static void Initialize();
	// 
	// 	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return CoreLogger; }
	// 	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return ClientLogger; }
	// private:
	// 	static std::shared_ptr<spdlog::logger> CoreLogger;
	// 	static std::shared_ptr<spdlog::logger> ClientLogger;
	// };
}

// Core log macros
#define LOG_CORE_DEBUG(...)    // ::EEngine::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define LOG_CORE_INFO(...)     // ::EEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)     // ::EEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)    // ::EEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...) // ::EEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define LOG_DEBUG(...)         // ::EEngine::Log::GetClientLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)          // ::EEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)          // ::EEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)         // ::EEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)      // ::EEngine::Log::GetClientLogger()->critical(__VA_ARGS__)