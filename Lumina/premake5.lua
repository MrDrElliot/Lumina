include "Dependencies.lua"


project "Lumina"
	kind "StaticLib"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")

	
	files
	{
		"Engine/EntryPoint.h",
		"Engine/Lumina.h",

		"Engine/Source/**.h",
		"Engine/Source/**.cpp",
		
		"Engine/ThirdParty/stb_image/**.h",
		
	    "Engine/ThirdParty/fastgltf/include/**.hpp",
	    "Engine/ThirdParty/fastgltf/src/**.cpp",

		"Engine/ThirdParty/glm/glm/**.hpp",
		"Engine/ThirdParty/glm/glm/**.cpp",
		"Engine/ThirdParty/glm/glm/**.inl",
		
		"Engine/ThirdParty/vk-bootstrap/src/**.h",
	    "Engine/ThirdParty/vk-bootstrap/src/**.cpp",
	    
	    "Engine/ThirdParty/vulkanmemoryallocator/**.h",
	    "Engine/ThirdParty/vulkanmemoryallocator/src/VmaUsage.cpp",
	    
	    "Engine/ThirdParty/imgui/backends/imgui_impl_glfw.h",
	    "Engine/ThirdParty/imgui/backends/imgui_impl_glfw.cpp",
	    
	    "Engine/ThirdParty/imgui/backends/imgui_impl_vulkan.h",
	    "Engine/ThirdParty/imgui/backends/imgui_impl_vulkan.cpp",
	  

	}


	includedirs
	{ 
		"Engine/",
		"Engine/ThirdParty/",
		"%{IncludeDir.spdlog}/",
		"%{IncludeDir.glfw}/",
		"%{IncludeDir.glm}/",
		"%{IncludeDir.imgui}/",
		"%{IncludeDir.stb_image}/",
		"%{IncludeDir.fastgltf}/",
		"%{IncludeDir.vkbootstrap}/src/",
		"%{IncludeDir.vulkan}/Include/",
		"%{IncludeDir.vulkan}/Lib/",
		"%{IncludeDir.vulkanmemoryallocator}/",

	}
	
	links { "GLFW" }

	defines
	 {
		 "GLM_FORCE_DEPTH_ZERO_TO_ONE",
		 "GLFW_INCLUDE_NONE",
	}

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
