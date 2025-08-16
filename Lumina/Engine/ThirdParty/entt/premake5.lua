
project "EnTT"
	kind "Utility"
    staticruntime "on"
	language "C++"

    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.hpp",
	}