include "Dependencies.lua"

project "LuminaEditor"
	kind "ConsoleApp"


	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")
	
	links
	 {
		"Lumina",
	  	"GLFW",
	  	"imgui",
	  	"%{VULKAN_SDK}/lib/vulkan-1.lib",  	
	  	"%{VULKAN_SDK}/lib/shaderc.lib",  	
	  	
	 }

	files
	{
		"Editor/**.h",
		"Editor/**.cpp",

	}


	includedirs
	{ 
		"Editor/",
		"../Lumina/Engine",
		"../Lumina/Engine/ThirdParty/spdlog/include/",
		"../Lumina/Engine/ThirdParty/GLFW/include/",
		"../Lumina/Engine/ThirdParty/GLFW/src/",
		"../Lumina/Engine/ThirdParty/GLM/",
		"../Lumina/Engine/ThirdParty/",
		"../Lumina/Engine/ThirdParty/nativefiledialog/src/include",
		"../Lumina/Engine/ThirdParty/stb_image/",
		"../Lumina/Engine/ThirdParty/json/include/",
		"../Lumina/Engine/ThirdParty/fastgltf/include/",
		"../Lumina/Engine/ThirdParty/fastgltf/src/",
        "../Lumina/Engine/ThirdParty/vulkanmemoryallocator/include/",
        "../Lumina/Engine/ThirdParty/vulkanmemoryallocator/src/",
        "../Lumina/Engine/ThirdParty/imgui/",
        "../Luumina/Engine/ThirdParty/shaderc/",
        "../Lumina/Engine/ThirdParty/SPIRV-Reflect/",
        "../Lumina/Engine/ThirdParty/json",
        
		"%{IncludeDir.shaderc}/",
		"%{IncludeDir.vulkan}/Include/",
		"%{IncludeDir.fastgltf}/Include",
	}


	defines { "GLM_FORCE_DEPTH_ZERO_TO_ONE", }

	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }

	filter "configurations:Debug"
		symbols "On"
		defines { "LE_DEBUG", "_DEBUG", }

	filter { "system:windows" }	
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP" }
