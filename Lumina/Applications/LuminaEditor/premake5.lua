include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Editor"
	kind "ConsoleApp"


	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediates/" .. outputdir .. "/%{prj.name}")
	
	links
	 {
		"Lumina",
	  	"GLFW",
	  	"imgui",
	  	"$(VULKAN_SDK)/lib/vulkan-1.lib",
	    "%{VULKAN_SDK}/lib/shaderc.lib",  	
	 }
	 
	files
	{
		"Editor/**.h",
		"Editor/**.cpp",
	}

	includedirs
	{ 
	    "Editor",
	    
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
		defines { "LE_PLATFORM_WINDOWS" }


	filter "configurations:Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP" }