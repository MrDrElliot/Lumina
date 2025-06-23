include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Sandbox"
	kind "ConsoleApp"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")    
		
	removedefines { }

		
	links
	{
		"Lumina",
    	"EA",
		"GLFW",
	}


	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"%{wks.location}/Intermediates/Reflection/Sandbox/**.h",
		"%{wks.location}/Intermediates/Reflection/Sandbox/**.cpp",

	}


	includedirs
	{ 
		"Source",
	    
	    "%{LuminaEngineDirectory}/Lumina/",
		"%{LuminaEngineDirectory}/Lumina/Engine/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/Runtime/",
	    "%{wks.location}/Intermediates/Reflection/Sandbox/",

	    reflection_directory();
		includedependencies();
		
	}


	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { "LE_DEBUG", "_DEBUG", }

	filter { "system:windows" }	
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		runtime "Release"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP" }