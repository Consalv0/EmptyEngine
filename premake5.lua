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

    files {
        "%{prj.location}/Source/**.h",
        "%{prj.location}/Source/**.inl",
        "%{prj.location}/Source/**.cpp",
    }

    includedirs {
        "%{prj.location}/Source",
        "%{prj.location}/Source/Runtime",
        "%{prj.location}/Source/Runtime/Public",
        "%{IncludeDir.SDL}/include"
    }      

    libdirs { 
        "%{prj.location}/Libraries",
        "%{LibrariesDir.SDL}/%{cfg.buildcfg}"
    }

    links {
        "SDL3.lib"
    }

    filter "system:windows"
        systemversion "latest"
        postbuildcommands {
        }
        defines {
            "EE_PLATFORM_WINDOWS",
            "EE_DLLEXPORT",
        }

    filter "configurations:Debug"
        defines { 
            "EE_DEBUG", "EE_ENABLE_ASSERTS"
        }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "EE_RELEASE"
        runtime "Release"
        optimize "on"

--project "SDL"
--    location "%{IncludeDir.SDL}"
--    kind "StaticLib"
--    language "C"
--    staticruntime "on"
--
--    targetdir ("%{prj.location}/Build/" .. outputdir)
--    objdir ("%{prj.location}/BinObjs/" .. outputdir)
--    
--    files {
--        "%{prj.location}/src/**.h",
--        "%{prj.location}/src/**.c",
--    }
--
--    includedirs {
--        "%{prj.location}/include",
--    }
--        
--    libdirs { 
--        "%{prj.location}/Libraries"
--    }
--
--    links {
--    }
--
--    filter "system:windows"
--        systemversion "latest"
--
--    filter "configurations:Debug"
--        runtime "Debug"
--        symbols "on"
--
--    filter "configurations:Release"
--        runtime "Release"
--        optimize "on"
--
--    filter "configurations:Shipping"
--        runtime "Release"
--        optimize "on"