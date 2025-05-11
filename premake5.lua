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
    
    removefiles {
        "%{prj.location}/Source/Platform/**"
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
        "spdlog",
        "VMA",
        "JoltPhysics"
    }
    
    flags { 
        "MultiProcessorCompile"
    }

    filter "system:emscripten"
        systemversion "latest"
        
        removeincludedirs {
            "%{IncludeDir.VulkanSDK}/include",
            "%{IncludeDir.VMA}/include"
        }

        removefiles {
            "%{prj.location}/Source/Runtime/Private/RHI/Vulkan/**",
            "%{prj.location}/Source/Runtime/Public/RHI/Vulkan/**"
        }

        removelinks {
            "VMA"
        }

        files {
            "%{prj.location}/Source/Platform/Web/**.h",
            "%{prj.location}/Source/Platform/Web/**.inl",
            "%{prj.location}/Source/Platform/Web/**.cpp",
        }

        -- libdirs {
        --     "%{LibrariesDir.SDL}/Build/%{cfg.system}/%{cfg.buildcfg}"
        -- }
-- 
        -- links {
        --     "libSDL3.a",
        -- }
        
        prebuildcommands {
            'emcmake cmake -DSDL_THREADS=ON -DCMAKE_BUILD_TYPE=%{cfg.buildcfg} -S %[%{LibrariesDir.SDL}] -B %[%{LibrariesDir.SDL}/Build/%{cfg.system}/%{cfg.buildcfg}] -G Ninja',
            'pushd %[%{LibrariesDir.SDL}/Build/%{cfg.system}/%{cfg.buildcfg}] && ninja && popd'
        }

        linkoptions{
            "-s USE_PTHREADS=1",
            "-s FULL_ES3=1",
            "-s MIN_WEBGL_VERSION=2", 
            "-s MAX_WEBGL_VERSION=2"
        }
        
        defines {
            "EE_PLATFORM_WEB",
        }

    filter "system:windows"
        systemversion "latest"
        buildoptions{ "/utf-8" }

        libdirs { 
            "%{LibrariesDir.SDL}/Build/%{cfg.system}/%{cfg.buildcfg}"
        }

        links {
            "vulkan-1.lib",
            "SDL3.lib",
        }

        files {
            "%{prj.location}/Source/Platform/Windows/**.h",
            "%{prj.location}/Source/Platform/Windows/**.inl",
            "%{prj.location}/Source/Platform/Windows/**.cpp",
        }
        
        prelinkcommands  { 
        }

        prebuildcommands {
            'cmake -S %[%{LibrariesDir.SDL}] -B %[%{LibrariesDir.SDL}/Build/%{cfg.system}]',
            'cmake --build %[%{LibrariesDir.SDL}/Build/%{cfg.system}] --config %[%{cfg.buildcfg}]',
            "{MKDIR} %[%{cfg.targetdir}]",
            "{COPY} %[%{LibrariesDir.SDL}/Build/%{cfg.system}/%{cfg.buildcfg}/*.dll] %[%{cfg.targetdir}/*.*]",
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

    filter "system:emscripten"
        kind "None"

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
        buildoptions{ "/utf-8" }

    filter "system:emscripten"
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
        "JPH_CROSS_PLATFORM_DETERMINISTIC"
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

    filter "system:emscripten"
        systemversion "latest"

    filter "system:windows"
        buildoptions{ "/arch:AVX2" }
        defines{ "JPH_USE_AVX2" }
        systemversion "latest"

    filter "configurations:Debug"
        defines{ "_DEBUG", "JPH_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines{ "NDEBUG" }
        runtime "Release"
        optimize "Speed"

-- externalproject "SDL"
--     location "%{IncludeDir.SDL}/VisualC/SDL"
--     uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
--     kind "SharedLib"
--     language "C++"