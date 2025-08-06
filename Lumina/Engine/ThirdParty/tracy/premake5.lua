include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Tracy"
	kind "SharedLib"
	language "C++"
	staticruntime "Off"

    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")

    defines
    {
		"TRACY_ENABLE",
    	"TRACY_CALLSTACK",
    	"TRACY_ON_DEMAND",
		"TRACY_EXPORTS",
    }

	buildoptions
	{
		"/W2",
	}

	files
	{
		"public/TracyClient.cpp",
	}
	
	includedirs
	{
		"public",
	}