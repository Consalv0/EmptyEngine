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
        "EMPTYENGINE_CORE",
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
        "%{IncludeDir.VMA}/include",
        "%{IncludeDir.spdlog}/include"
    }

    libdirs { 
        "%{prj.location}/Libraries",
        "%{LibrariesDir.VulkanSDK}",
    }

    links {
        "spdlog",
        "VMA",
        "vulkan-1.lib",
        "SDL3.lib",
    }

    filter "system:windows"
        systemversion "latest"

        libdirs { 
            "%{LibrariesDir.SDL}/VisualC/SDL/%{outputdir}"
        }
        
        prelinkcommands  { 
            "{MKDIR} %[%{LibrariesDir.SDL}/VisualC/SDL/%{outputdir}]",
            "IF EXIST %[%{wks.location}/x64/%{cfg.buildcfg}/] ({MOVE} %[%{wks.location}/x64/%{cfg.buildcfg}/*.*] %[%{LibrariesDir.SDL}/VisualC/SDL/%{outputdir}/])",
            "IF EXIST %[%{wks.location}/x64/] ({RMDIR} %[%{wks.location}/x64/])"
        }

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

project "VMA"
    location "%{IncludeDir.VMA}"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    pic "on"

    targetdir ("%{prj.location}/Build/" .. outputdir)
    objdir ("%{prj.location}/BinObjs/" .. outputdir)
    
    defines {
    }

    files {
        "%{prj.location}/include/**.h",
        "%{prj.location}/src/VmaUsage.h",
        "%{prj.location}/src/VmaUsage.cpp"
    }

    includedirs {
        "%{prj.location}/src",
        "%{prj.location}/include",
        "%{IncludeDir.VulkanSDK}/include",
    }

    libdirs {
        "%{LibrariesDir.VulkanSDK}",
    }

    links {
        "vulkan-1.lib",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
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

externalproject "SDL"
    location "%{IncludeDir.SDL}/VisualC/SDL"
    uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
    kind "SharedLib"
    language "C++"