include "Dependencies.lua"


project "Lumina"
	kind "StaticLib"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")

	
	files
	{
		"Engine/**.h",
		"Engine/**.cpp",

		"Engine/**.hpp",
		"Engine/**.cpp",

		"Engine/**.inl",

	}


	includedirs
	{ 
		"Engine/",
		"Engine/Source/ThirdParty/",
		"%{IncludeDir.spdlog}/",
		"%{IncludeDir.glfw}/",
		"%{IncludeDir.glm}/",
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
