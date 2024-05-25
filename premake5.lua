include "dependencies.lua"

project "EmptyEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{prj.location}/Build/" .. outputdir)
    objdir ("%{prj.location}/BinObjs/" .. outputdir)

    pchsource "%{prj.location}/Source/Runtime/Private/CoreMinimal.cpp"
    pchheader "CoreMinimal.h"

    defines {
        --"FIXMATH_NO_ROUNDING",
        --"FIXMATH_NO_OVERFLOW",
    }

    files {
        "%{prj.location}/Source/**.h",
        "%{prj.location}/Source/**.inl",
        "%{prj.location}/Source/**.cpp",
    }

    includedirs {
        "%{prj.location}/Source",
        "%{prj.location}/Source/Runtime",
        "%{prj.location}/Source/Runtime/Public",
        "%{IncludeDir.SDL}/include",
        "%{IncludeDir.VulkanSDK}/include",
        "%{IncludeDir.spdlog}/include"
    }      

    libdirs { 
        "%{prj.location}/Libraries",
        "%{LibrariesDir.VulkanSDK}",
        "%{LibrariesDir.SDL}/%{cfg.buildcfg}"
    }

    links {
        "spdlog",
        "vulkan-1.lib",
        "SDL3.lib",
    }

    filter "system:windows"
        systemversion "latest"
        postbuildcommands {
        }
        defines {
            "EE_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
        defines { 
            "EE_DEBUG", "EE_ENABLE_ASSERTS"
        }
        runtime "Debug"
        optimize "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "EE_RELEASE"
        runtime "Release"
        optimize "on"

project "spdlog"
    location "%{IncludeDir.spdlog}"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    pic "on"

    targetdir ("%{prj.location}/Build/" .. outputdir)
    objdir ("%{prj.location}/BinObjs/" .. outputdir)
    
    defines {
        "SPDLOG_WCHAR_TO_UTF8_SUPPORT",
        "SPDLOG_NO_DATETIME",
        "SPDLOG_COMPILED_LIB"
    }

    files {
        "%{prj.location}/include/**.h",
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp"
    }

    includedirs {
        "%{prj.location}/src",
        "%{prj.location}/include",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"