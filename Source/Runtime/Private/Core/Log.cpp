
#include "CoreMinimal.h"
#include "Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>


namespace EE
{
    std::shared_ptr<spdlog::logger> Log::CoreLogger;
    std::shared_ptr<spdlog::logger> Log::ClientLogger;

    void Log::Initialize()
    {
        spdlog::set_pattern( "%^%n[%l] %v%$" );
        CoreLogger = spdlog::stdout_color_mt( "*" );
        CoreLogger->set_level( spdlog::level::trace );

        ClientLogger = spdlog::stdout_color_mt( "~" );
        ClientLogger->set_level( spdlog::level::trace );
    }
}