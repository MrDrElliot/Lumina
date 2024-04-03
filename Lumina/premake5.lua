include "Dependencies.lua"


project "Lumina"
	kind "StaticLib"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")

	
	files
	{
		"Engine/EntryPoint.h",
		"Engine/Lumina.h",

		"Engine/Source/**.h",
		"Engine/Source/**.cpp",
		
		"Engine/ThirdParty/glm/glm/**.hpp",
		"Engine/ThirdParty/glm/glm/**.cpp",
		"Engine/ThirdParty/glm/glm/**.inl",
	}


	includedirs
	{ 
		"Engine/",
		"Engine/ThirdParty/",
		"%{IncludeDir.spdlog}/",
		"%{IncludeDir.glfw}/",
		"%{IncludeDir.glm}/",
		"%{IncludeDir.vulkan}/Include/",
		"%{IncludeDir.vulkan}/Lib/",

	}
	
	links { "GLFW" }

	defines
	 {
		 "GLM_FORCE_DEPTH_ZERO_TO_ONE",
		 "GLFW_INCLUDE_NONE",
	}

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
