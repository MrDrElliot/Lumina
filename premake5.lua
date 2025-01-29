workspace "Lumina"
	configurations { "Debug", "Release", "Shipping" }
	targetdir "Build"
	startproject "LuminaEditor"
	conformancemode "On"

	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	flags  { "MultiProcessorCompile" }
	


	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "action:vs"
	
	filter "language:C++ or language:C"
		architecture "x86_64"

	filter "configurations:Debug"
		optimize "Off"
		symbols "On"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Shipping"
		optimize "Full"
		symbols "Off"

	filter "system:windows"
		buildoptions 
		{ 
            "/EHsc",
            "/Zc:preprocessor",
            "/Zc:__cplusplus",
            "/W2"
        }

	group "Dependencies"
		include "Lumina/Engine/ThirdParty/glfw"
		include "Lumina/Engine/ThirdParty/imgui"
	group ""

	group "Core"
		include "Lumina"
	group ""

	group "Applications"
		include "Lumina/Applications/LuminaEditor"
		include "Lumina/Applications/Sandbox"
	group ""