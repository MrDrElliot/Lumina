include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Editor"
	kind "ConsoleApp"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")   
	
	defines
	{ 
		"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
		"TRACY_ENABLE",
    	"TRACY_CALLSTACK",
    	"TRACY_ON_DEMAND",
		"TRACY_IMPORTS",
	}

	links
	 {
		"Lumina",
		"ImGui",
    	"EA",
    	"EnkiTS",
		"Tracy",
		"XXHash",
		"RPMalloc",
		"EnTT",
	 }
	 
	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"%{wks.location}/Intermediates/Reflection/Editor/**.h",
        "%{wks.location}/Intermediates/Reflection/Editor/**.cpp",
	}

	includedirs
	{ 
	    "Source",
	    
	    "%{LuminaEngineDirectory}/Lumina/",
		"%{LuminaEngineDirectory}/Lumina/Engine/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/Runtime/",
	    "%{LuminaEngineDirectory}/Intermediates/Reflection/Lumina/",

	    reflection_directory();
		includedependencies();
	}
	 
	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }
		
	filter { "system:windows" }	
		flags { "NoRuntimeChecks", "NoIncrementalLink" }
		defines { "LE_PLATFORM_WINDOWS" }

	filter "configurations:Debug"
		symbols "On"
		defines { "LE_DEBUG", "_DEBUG", }



	filter "configurations:Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP" }
