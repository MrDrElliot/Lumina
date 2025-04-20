include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "EA"
	kind "StaticLib"
    staticruntime "on"
	language "C++"

    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")

    defines
    {
        "LUMINA_ENGINE",
    }

	files
	{
		"**.h",
		"**.cpp"
	}
	
	includedirs
	{
		"EABase",
		"EASTL",
		"EABase/include/Common",
		"EASTL/include/",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
	defines "LE_DEBUG"
	buildoptions "/MDd"
	symbols "On"
	
	filter "configurations:Release"
	defines "LE_RELEASE"
	buildoptions "/MD"
	optimize "On"
