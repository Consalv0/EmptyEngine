include "dependencies.lua"

project "EmptyEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"

    targetdir ("%{prj.location}/Build/" .. outputdir)
    objdir ("%{prj.location}/BinObjs/" .. outputdir)

    pchsource "%{prj.location}/Source/Runtime/Private/CoreMinimal.cpp"
    pchheader "CoreMinimal.h"
    inlining "Explicit"

    defines {
        "EMPTYENGINE_CORE",
        --"EE_FIXMATH_NO_ROUNDING",
        --"EE_FIXMATH_NO_OVERFLOW",
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
        "%{IncludeDir.JoltPhysics}",
        "%{IncludeDir.SDL}/include",
        "%{IncludeDir.VulkanSDK}/include",
        "%{IncludeDir.VMA}/include",
        "%{IncludeDir.spdlog}/include",
        "%{IncludeDir.stb}",
    }

    libdirs { 
        "%{prj.location}/Libraries",
        "%{LibrariesDir.VulkanSDK}",
    }

    links {
        "Dwmapi.lib",
        "spdlog",
        "VMA",
        "JoltPhysics",
        "vulkan-1.lib",
        "SDL3.lib",
    }
    
    flags { 
        "MultiProcessorCompile"
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
        symbols "On"

    filter "configurations:Release"
        defines "EE_RELEASE"
        runtime "Release"
        optimize "Speed"

project "VMA"
    location "%{IncludeDir.VMA}"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"
    pic "On"

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
    
    flags { 
        "MultiProcessorCompile"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "Speed"

project "spdlog"
    location "%{IncludeDir.spdlog}"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"
    pic "On"

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
    
    flags { 
        "MultiProcessorCompile"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "Speed"

project "JoltPhysics"
    location "%{IncludeDir.JoltPhysics}"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    pic "On"

    targetdir ("%{prj.location}/Build/" .. outputdir)
    objdir ("%{prj.location}/BinObjs/" .. outputdir)
    
    defines {
        "JOLT_PHYSICS_SRC_FILES=%{prj.location}/Jolt/Jolt.natvis",
        "JPH_CROSS_PLATFORM_DETERMINISTIC",
        "JPH_USE_AVX2"
    }

    files {
        "%{prj.location}/Jolt/**.h",
        "%{prj.location}/Jolt/**.inl",
        "%{prj.location}/Jolt/**.cpp"
    }

    removefiles {
		"%{prj.location}/Jolt/ObjectStream/GetPrimitiveTypeOfType.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStream.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStream.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamBinaryIn.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamBinaryIn.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamBinaryOut.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamBinaryOut.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamIn.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamIn.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamOut.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamOut.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamTextIn.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamTextIn.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamTextOut.cpp",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamTextOut.h",
		"%{prj.location}/Jolt/ObjectStream/ObjectStreamTypes.h",
		"%{prj.location}/Jolt/ObjectStream/SerializableAttribute.h",
		"%{prj.location}/Jolt/ObjectStream/SerializableAttributeEnum.h",
		"%{prj.location}/Jolt/ObjectStream/SerializableAttributeTyped.h",
		"%{prj.location}/Jolt/ObjectStream/TypeDeclarations.cpp",
		"%{prj.location}/Jolt/ObjectStream/TypeDeclarations.h",
    }

    includedirs {
        "%{prj.location}",
    }
    
    flags { 
        "MultiProcessorCompile"
    }

    filter "system:windows"
        buildoptions{ "/arch:AVX2" }
        systemversion "latest"

    filter "configurations:Debug"
        defines{ "_DEBUG", "JPH_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines{ "NDEBUG" }
        runtime "Release"
        optimize "Speed"

externalproject "SDL"
    location "%{IncludeDir.SDL}/VisualC/SDL"
    uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
    kind "SharedLib"
    language "C++"