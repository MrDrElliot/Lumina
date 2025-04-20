include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Editor"
	kind "ConsoleApp"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")   
	
	links
	 {
		"Lumina",
		"DbgHelp",
		"EA",
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
	    
	    reflection_directory();
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