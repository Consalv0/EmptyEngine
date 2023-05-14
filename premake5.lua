workspace "EmptyEngine"
	architecture "x64"
    startproject "EmptyEngine"

	configurations
	{
		"Debug",
		"Release",
		"Shipping"
	}
	---%{cfg.system}
	outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"

	-- Include directories relative to root folder (solution directory)
	IncludeDir = {}

	project "EmptyEngine"
		location "Engine"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("%{prj.location}/Build/" .. outputdir)
		objdir ("%{prj.location}/BinObjs/" .. outputdir)

		pchsource "%{prj.location}/Source/Runtime/Public/CoreMinimal.cpp"
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
		}      

	    libdirs { 
	        "%{prj.location}/Libraries"
	    }

	    links {
	        --"SDL2.lib"
	    }

		filter "system:windows"
			systemversion "latest"

			defines {
	            "ES_PLATFORM_WINDOWS",
				"ES_DLLEXPORT",
			}

		filter "configurations:Debug"
			defines { 
				"ES_DEBUG", "ES_ENABLE_ASSERTS"
			}
			runtime "Debug"
	        symbols "on"

		filter "configurations:Release"
			defines "ES_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations:Shipping"
			defines "ES_SHIPPING"
			runtime "Release"
			optimize "on"

	project "dos"
		location "External/dos"
		kind "StaticLib"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"

		targetdir ("%{prj.location}/Build/" .. outputdir)
		objdir ("%{prj.location}/BinObjs/" .. outputdir)

		files {
			"%{prj.location}/Source/**.h",
			"%{prj.location}/Source/**.c",
		}

		includedirs {
			"%{prj.location}/Include",
		}
		
		libdirs { 
			"%{prj.location}/Libraries"
		}

		links {
		}

		filter "system:windows"
			systemversion "latest"

		defines {
            "ES_PLATFORM_WINDOWS",
			"ES_DLLEXPORT",
		}

		filter "configurations:Debug"
			defines { 
				"ES_DEBUG", "ES_ENABLE_ASSERTS"
			}
			runtime "Debug"
        	symbols "on"

		filter "configurations:Release"
			defines "ES_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations:Shipping"
			defines "ES_SHIPPING"
			runtime "Release"
			optimize "on"