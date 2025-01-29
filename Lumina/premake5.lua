include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Lumina"
	kind "StaticLib"


	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("../Intermediates/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"Engine/Source/**.h",
		"Engine/Source/**.cpp",
		
		"Engine/ThirdParty/stb_image/**.h",

		"Engine/ThirdParty/glm/glm/**.hpp",
		"Engine/ThirdParty/glm/glm/**.cpp",
		"Engine/ThirdParty/glm/glm/**.inl",
		
		"Engine/ThirdParty/vk-bootstrap/src/**.h",
	    "Engine/ThirdParty/vk-bootstrap/src/**.cpp",

		"Engine/ThirdParty/nvrhi-main/src/**.h",
	    "Engine/ThirdParty/nvrhi-main/src/**.cpp",
	    
        "Engine/ThirdParty/json/include/**.h",
        "Engine/ThirdParty/json/src/**.cpp",

		"Engine/ThirdParty/ImGuizmo/**.h",
        "Engine/ThirdParty/ImGuizmo/**.cpp",
       
		
        "Engine/ThirdParty/yaml-cpp/include/**.h",
        "Engine/ThirdParty/yaml-cpp/src/**.h",
        "Engine/ThirdParty/yaml-cpp/src/**.cpp",
        
        
	    "Engine/ThirdParty/SPIRV-Reflect/**.h",
	    "Engine/ThirdParty/SPIRV-Reflect/**.c",
	    "Engine/ThirdParty/SPIRV-Reflect/**.cpp",
	    
	    "Engine/ThirdParty/EA/**.h",
        "Engine/ThirdParty/EA/**.cpp",

	    "Engine/ThirdParty/fastgltf/src/**.cpp",
	    "Engine/ThirdParty/fastgltf/deps/simdjson/**.h",
        "Engine/ThirdParty/fastgltf/deps/simdjson/**.cpp",
        
        "Engine/ThirdParty/stb_image/**.h",
       
	    "Engine/ThirdParty/vulkanmemoryallocator/**.h",
	    "Engine/ThirdParty/vulkanmemoryallocator/src/VmaUsage.cpp",
	    
	    "Engine/ThirdParty/imgui/backends/imgui_impl_glfw.h",
	    "Engine/ThirdParty/imgui/backends/imgui_impl_glfw.cpp",
	    
	    "Engine/ThirdParty/imgui/backends/imgui_impl_vulkan.h",
	    "Engine/ThirdParty/imgui/backends/imgui_impl_vulkan.cpp",

		"Engine/ThirdParty/entt/src/**.hpp",
	  	"Engine/ThirdParty/entt/src/**.cpp",

	}


	includedirs
	{ 
		"Engine",
		"Engine/Source",
		"Engine/Source/Runtime",
		"Engine/ThirdParty/",
		
		includedependencies();
	}
	
	links
	 {
	    "GLFW",
	    "ImGui",
	  	"%{VULKAN_SDK}/lib/vulkan-1.lib",  	
	    "%{VULKAN_SDK}/lib/shaderc.lib",  	
	 }

	defines
	 {
		 "GLM_FORCE_DEPTH_ZERO_TO_ONE",
		 "GLFW_INCLUDE_NONE",
		 "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
	 }

	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }


	filter { "system:windows" }	
		defines { "LE_PLATFORM_WINDOWS", }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }
		links { "Dbghelp", }

	filter "configurations:Debug"
		symbols "On"
		editandcontinue "Off"
		defines { "LE_DEBUG", "_DEBUG", }

	filter "configurations:Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP" }
