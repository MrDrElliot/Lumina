include "Dependencies.lua"

project "Sandbox"
	kind "ConsoleApp"


	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")
	
	links
	 {
		"Lumina",
	  	"GLFW",
	  	"$(VULKAN_SDK)/lib/vulkan-1.lib",
	 }

	files
	{
		"Source/**.h",
		"Source/**.cpp",

	}


	includedirs
	{ 
		"Source/",
		"../Lumina/Engine",
		"../Lumina/Engine/ThirdParty/spdlog/include/",
		"../Lumina/Engine/ThirdParty/GLFW/include/",
		"../Lumina/Engine/ThirdParty/GLFW/src/",
		"../Lumina/Engine/ThirdParty/GLM/",
		"%{IncludeDir.vulkan}/Include/",
		"%{IncludeDir.vulkan}/Lib/",
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
