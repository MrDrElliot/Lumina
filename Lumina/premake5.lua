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

		"Engine/ThirdParty/glm/glm/**.hpp",
		"Engine/ThirdParty/glm/glm/**.cpp",
		"Engine/ThirdParty/glm/glm/**.inl",
		
		"Engine/ThirdParty/vk-bootstrap/src/**.h",
	    "Engine/ThirdParty/vk-bootstrap/src/**.cpp",
	    
	    "Engine/ThirdParty/shaderc/**.h",
        "Engine/ThirdParty/shaderc/**.cc",
        
	    "Engine/ThirdParty/SPIRV-Reflect/**.h",
	    "Engine/ThirdParty/SPIRV-Reflect/**.c",
	    "Engine/ThirdParty/SPIRV-Reflect/**.cpp",

	    "Engine/ThirdParty/fastgltf/src/**.cpp",
	    "Engine/ThirdParty/fastgltf/deps/simdjson/**.h",
        "Engine/ThirdParty/fastgltf/deps/simdjson/**.c",
       
       
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
		"%{IncludeDir.SPIRV_Reflect}/include",
		"%{IncludeDir.imgui}/",
		"%{IncludeDir.stb_image}/",
		"%{IncludeDir.fastgltf}/",
		"%{IncludeDir.vkbootstrap}/src/",
		"%{IncludeDir.vulkan}/Include/",
		"%{IncludeDir.shaderc}/",
		"%{IncludeDir.vulkanmemoryallocator}/",

	}
	
	links
	 {
	    "GLFW",
	  	"%{VULKAN_SDK}/lib/vulkan-1.lib",  	
	  	"%{VULKAN_SDK}/lib/shaderc.lib",  	
	 }

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
