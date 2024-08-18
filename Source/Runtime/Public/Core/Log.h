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
        static std::shared_ptr<spdlog::logger> GCoreLogger;
        static std::shared_ptr<spdlog::logger> GClientLogger;

    public:
        static void Initialize();
    };
}

// Core log macros
#define EE_LOG_CORE_DEBUG(...)    SPDLOG_LOGGER_DEBUG(::EE::Log::GCoreLogger, __VA_ARGS__)
#define EE_LOG_CORE_INFO(...)     SPDLOG_LOGGER_INFO(::EE::Log::GCoreLogger, __VA_ARGS__)
#define EE_LOG_CORE_WARN(...)     SPDLOG_LOGGER_WARN(::EE::Log::GCoreLogger, __VA_ARGS__)
#define EE_LOG_CORE_ERROR(...)    SPDLOG_LOGGER_ERROR(::EE::Log::GCoreLogger, __VA_ARGS__)
#define EE_LOG_CORE_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::EE::Log::GCoreLogger, __VA_ARGS__)

#ifdef EE_ENABLE_ASSERTS
    #define EE_CORE_ASSERT(X, ...) { if(!(X)) { EE_LOG_CORE_CRITICAL(L"Assertion Failed: " __VA_ARGS__); __debugbreak(); } }
    #define EE_ASSERT(X, ...) { if(!(X)) { EE_LOG_CRITICAL(L"Assertion Failed: " __VA_ARGS__); __debugbreak(); } }
#else
    #define EE_CORE_ASSERT(X, ...) (X)
    #define EE_ASSERT(X, ...) (X)
#endif

// Client log macros
#define EE_LOG_DEBUG(...)         SPDLOG_LOGGER_DEBUG(::EE::Log::GClientLogger, __VA_ARGS__)
#define EE_LOG_INFO(...)          SPDLOG_LOGGER_INFO(::EE::Log::GClientLogger, __VA_ARGS__)
#define EE_LOG_WARN(...)          SPDLOG_LOGGER_WARN(::EE::Log::GClientLogger, __VA_ARGS__)
#define EE_LOG_ERROR(...)         SPDLOG_LOGGER_ERROR(::EE::Log::GClientLogger, __VA_ARGS__)
#define EE_LOG_CRITICAL(...)      SPDLOG_LOGGER_CRITICAL(::EE::Log::GClientLogger, __VA_ARGS__)