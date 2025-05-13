workspace "Lumina"
	configurations { "Debug", "Release", "Shipping" }
	targetdir "Build"
	startproject "Editor"
	conformancemode "On"

	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	flags  
	{
		"MultiProcessorCompile", 
	}
		
	defines
	{
		"EASTL_USER_DEFINED_ALLOCATOR=1",
		"_CRT_SECURE_NO_WARNINGS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"IMGUI_DEFINE_MATH_OPERATORS",
	}

	filter "action:vs"
	
	filter "language:C++ or language:C"
		architecture "x86_64"

	filter "configurations:Debug"
		runtime "Debug"
		optimize "Off"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Shipping"
		runtime "Release"
		optimize "Full"
		symbols "Off"

	filter "system:windows"
		buildoptions 
		{
            "/EHsc",
            "/Zc:preprocessor",
            "/Zc:__cplusplus",
            "/W2",
            "/MP",
            "/Zm2000",
        }

	group "Dependencies"
		include "Lumina/Engine/ThirdParty/EA"
		include "Lumina/Engine/ThirdParty/glfw"
		include "Lumina/Engine/ThirdParty/imgui"
	group ""

	group "Core"
		include "Lumina"
	group ""

	group "Applications"
		include "Lumina/Applications/LuminaEditor"
		include "Lumina/Applications/Reflector"
		include "Lumina/Applications/Sandbox"
	group ""