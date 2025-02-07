include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Sandbox"
	kind "ConsoleApp"


	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediates/" .. outputdir .. "/%{prj.name}")
		
	removedefines { }
		
	links
	 {
		"Lumina",
	  	"GLFW",
	  	"imgui",
	  	"$(VULKAN_SDK)/lib/vulkan-1.lib",
	 }

	files
	{
		"Source/**.h",
		"Source/**.cpp",

	}


	includedirs
	{ 
		"Source",
	    
	    "%{LuminaEngineDirectory}/Lumina/",
		"%{LuminaEngineDirectory}/Lumina/Engine/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/Runtime/",
		includedependencies();
		
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