
project "RPMalloc"
	kind "StaticLib"
    staticruntime "on"
	language "C++"

    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")

    defines
    {
        "LUMINA_ENGINE",
    }
	
	buildoptions
	{
		"/W2",
	}

	files
	{
		"**.h",
		"**.c",
	}