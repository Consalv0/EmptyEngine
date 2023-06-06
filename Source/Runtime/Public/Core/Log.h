#pragma once

// #define SPDLOG_WCHAR_TO_UTF8_SUPPORT
// #define SPDLOG_NO_DATETIME
// #include <spdlog/spdlog.h>
// #include <spdlog/fmt/ostr.h>

namespace EE 
{
	class Log {
	public:
		static void Initialize();
	
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> CoreLogger;
		static std::shared_ptr<spdlog::logger> ClientLogger;
	};
}

// Core log macros
#define EE_LOG_CORE_DEBUG(...)    // ::EE::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define EE_LOG_CORE_INFO(...)     // ::EE::Log::GetCoreLogger()->info(__VA_ARGS__)
#define EE_LOG_CORE_WARN(...)     // ::EE::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define EE_LOG_CORE_ERROR(...)    // ::EE::Log::GetCoreLogger()->error(__VA_ARGS__)
#define EE_LOG_CORE_CRITICAL(...) // ::EE::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define EE_LOG_DEBUG(...)         // ::EE::Log::GetClientLogger()->debug(__VA_ARGS__)
#define EE_LOG_INFO(...)          // ::EE::Log::GetClientLogger()->info(__VA_ARGS__)
#define EE_LOG_WARN(...)          // ::EE::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EE_LOG_ERROR(...)         // ::EE::Log::GetClientLogger()->error(__VA_ARGS__)
#define EE_LOG_CRITICAL(...)      // ::EE::Log::GetClientLogger()->critical(__VA_ARGS__)