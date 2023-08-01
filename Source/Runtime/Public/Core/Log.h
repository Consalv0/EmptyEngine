#pragma once

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_NO_DATETIME
#define SPDLOG_COMPILED_LIB
#include <spdlog/spdlog.h>

namespace EE
{
    class Log
    {
    public:
        static std::shared_ptr<spdlog::logger> CoreLogger;
        static std::shared_ptr<spdlog::logger> ClientLogger;

    public:
        static void Initialize();
    };
}

#ifdef EMPTYENGINE_CORE_LOG

// Core log macros
#define EE_LOG_CORE_DEBUG(...)    ::EE::Log::CoreLogger->debug(__VA_ARGS__)
#define EE_LOG_CORE_INFO(...)     ::EE::Log::CoreLogger->info(__VA_ARGS__)
#define EE_LOG_CORE_WARN(...)     ::EE::Log::CoreLogger->warn(__VA_ARGS__)
#define EE_LOG_CORE_ERROR(...)    ::EE::Log::CoreLogger->error(__VA_ARGS__)
#define EE_LOG_CORE_CRITICAL(...) ::EE::Log::CoreLogger->critical(__VA_ARGS__)

#ifdef EE_ENABLE_ASSERTS
#define EE_CORE_ASSERT(X, ...) { if(!(X)) { EE_LOG_CORE_CRITICAL(L"Assertion Failed: " __VA_ARGS__); __debugbreak(); } }
#define EE_ASSERT(X, ...) { if(!(X)) { EE_LOG_CRITICAL(L"Assertion Failed: " __VA_ARGS__); __debugbreak(); } }

#else
#define EE_CORE_ASSERT(X, ...)
#define EE_ASSERT(X, ...)
#endif

#else

#define EE_CORE_ASSERT(X, ...)
#define EE_ASSERT(X, ...)

// Core log macros
#define EE_LOG_CORE_DEBUG(...)    
#define EE_LOG_CORE_INFO(...)     
#define EE_LOG_CORE_WARN(...)     
#define EE_LOG_CORE_ERROR(...)    
#define EE_LOG_CORE_CRITICAL(...) 

#endif // EMPTYENGINE_CORE_LOG

// Client log macros
#define EE_LOG_DEBUG(...)         ::EE::Log::ClientLogger->debug(__VA_ARGS__)
#define EE_LOG_INFO(...)          ::EE::Log::ClientLogger->info(__VA_ARGS__)
#define EE_LOG_WARN(...)          ::EE::Log::ClientLogger->warn(__VA_ARGS__)
#define EE_LOG_ERROR(...)         ::EE::Log::ClientLogger->error(__VA_ARGS__)
#define EE_LOG_CRITICAL(...)      ::EE::Log::ClientLogger->critical(__VA_ARGS__)