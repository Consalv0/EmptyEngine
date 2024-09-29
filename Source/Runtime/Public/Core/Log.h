#pragma once

#ifndef EE_LOG_LEVEL_FLAGS
    #define EE_LOG_LEVEL_FLAGS EE_LOG_LEVEL_DEBUG_BIT | EE_LOG_LEVEL_INFO_BIT | EE_LOG_LEVEL_WARN_BIT | EE_LOG_LEVEL_ERROR_BIT | EE_LOG_LEVEL_CRITICAL_BIT
#endif

#ifndef EE_CORE_LOG_LEVEL_FLAGS
    #define EE_CORE_LOG_LEVEL_FLAGS EE_LOG_LEVEL_DEBUG_BIT | EE_LOG_LEVEL_INFO_BIT | EE_LOG_LEVEL_WARN_BIT | EE_LOG_LEVEL_ERROR_BIT | EE_LOG_LEVEL_CRITICAL_BIT
#endif

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_NO_DATETIME
#define SPDLOG_COMPILED_LIB

#ifdef EE_ENABLE_ASSERTS
#include <assert.h>
#endif
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

        static void Shotdown();
    };
}

#ifdef EMPTYENGINE_CORE_LOG

// Core log macros
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_DEBUG_BIT ) > 0)
#define EE_LOG_DEBUG(...)  if ( EE::Log::GCoreLogger )      SPDLOG_LOGGER_DEBUG(EE::Log::GCoreLogger, __VA_ARGS__)
#else
#define EE_LOG_DEBUG(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_INFO_BIT ) > 0)
#define EE_LOG_INFO(...)  if ( EE::Log::GCoreLogger )       SPDLOG_LOGGER_INFO(EE::Log::GCoreLogger, __VA_ARGS__)
#else
#define EE_LOG_INFO(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_WARN_BIT ) > 0)
#define EE_LOG_WARN(...) if ( EE::Log::GCoreLogger )        SPDLOG_LOGGER_WARN(EE::Log::GCoreLogger, __VA_ARGS__)
#else
#define EE_LOG_WARN(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_ERROR_BIT ) > 0)
#define EE_LOG_ERROR(...) if ( EE::Log::GCoreLogger )       SPDLOG_LOGGER_ERROR(EE::Log::GCoreLogger, __VA_ARGS__)
#else
#define EE_LOG_ERROR(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_CRITICAL_BIT ) > 0)
#define EE_LOG_CRITICAL(...) if ( EE::Log::GCoreLogger )    SPDLOG_LOGGER_CRITICAL(EE::Log::GCoreLogger, __VA_ARGS__)
#else
#define EE_LOG_CRITICAL(...) void(0)
#endif

#else

// Client log macros
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_DEBUG_BIT ) > 0)
#define EE_LOG_DEBUG(...) if ( EE::Log::GClientLogger )    SPDLOG_LOGGER_DEBUG(EE::Log::GClientLogger, __VA_ARGS__)
#else
#define EE_LOG_DEBUG(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_INFO_BIT ) > 0)
#define EE_LOG_INFO(...) if ( EE::Log::GClientLogger )     SPDLOG_LOGGER_INFO(EE::Log::GClientLogger, __VA_ARGS__)
#else
#define EE_LOG_INFO(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_WARN_BIT ) > 0)
#define EE_LOG_WARN(...) if ( EE::Log::GClientLogger )     SPDLOG_LOGGER_WARN(EE::Log::GClientLogger, __VA_ARGS__)
#else
#define EE_LOG_WARN(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_ERROR_BIT ) > 0)
#define EE_LOG_ERROR(...) if ( EE::Log::GClientLogger )     SPDLOG_LOGGER_ERROR(EE::Log::GClientLogger, __VA_ARGS__)
#else
#define EE_LOG_ERROR(...) void(0)
#endif
#if ((EE_LOG_LEVEL_FLAGS & EE_LOG_LEVEL_CRITICAL_BIT ) > 0)
#define EE_LOG_CRITICAL(...) if ( EE::Log::GClientLogger )  SPDLOG_LOGGER_CRITICAL(EE::Log::GClientLogger, __VA_ARGS__)
#else
#define EE_LOG_CRITICAL(...) void(0)
#endif

#endif

#ifdef EE_ENABLE_ASSERTS
#define EE_ASSERT(X, ...) { if(!(X)) { EE_LOG_CRITICAL(L"Assertion Failed: " __VA_ARGS__); EE_BREAKPOINT; } }
#define EE_ASSERT(X, ...) { if(!(X)) { EE_LOG_CRITICAL(L"Assertion Failed: " __VA_ARGS__); EE_BREAKPOINT; } }
#else
#define EE_ASSERT(X, ...) (X)
#define EE_ASSERT(X, ...) (X)
#endif