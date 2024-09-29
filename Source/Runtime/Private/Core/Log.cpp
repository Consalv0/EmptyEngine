
#include "CoreMinimal.h"
#include "Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace EE
{
    std::shared_ptr<spdlog::logger> Log::GCoreLogger;
    std::shared_ptr<spdlog::logger> Log::GClientLogger;

    void Log::Initialize()
    {
        spdlog::set_pattern( "%^%n[%L] %v%$" );
        GCoreLogger = spdlog::stdout_color_mt( "*" );
        GCoreLogger->set_level( spdlog::level::trace );

        GClientLogger = spdlog::stdout_color_mt( "~" );
        GClientLogger->set_level( spdlog::level::trace );
    }

    void Log::Shotdown()
    {
        spdlog::drop( GCoreLogger->name() );
        GCoreLogger.reset();
        spdlog::drop( GClientLogger->name() );
        GClientLogger.reset();
    }
}