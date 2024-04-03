
project "Lumina"
	kind "StaticLib"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")

	IncludeDir = {}
	IncludeDir["spdlog"] = "Engine/Source/ThirdParty/spdlog/include"
	IncludeDir["glfw"] = "Engine/Source/ThirdParty/GLFW/include"

	
	files
	{
		"Engine/**.h",
		"Engine/**.cpp",
		"Engine/**.h",
		"Engine/**.cpp",
	}


	includedirs
	{ 
		"Engine/",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glfw}",
	}

	defines { "GLM_FORCE_DEPTH_ZERO_TO_ONE", }

	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }

	filter "configurations:Debug"
		symbols "On"
		defines { "LE_DEBUG", "_DEBUG", }

	filter { "system:windows" }	
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP" }
