include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Lumina"
	kind "SharedLib"
		
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")    
	
    	
	files
	{
		"Engine/Source/**.h",
		"Engine/Source/**.cpp",
		
		"%{wks.location}/Intermediates/Reflection/Lumina/**.h",
		"%{wks.location}/Intermediates/Reflection/Lumina/**.cpp",
		
		"Engine/ThirdParty/stb_image/**.h",

		"Engine/ThirdParty/glm/glm/**.hpp",
		"Engine/ThirdParty/glm/glm/**.cpp",
		"Engine/ThirdParty/glm/glm/**.inl",
		
		"Engine/ThirdParty/vk-bootstrap/src/**.h",
	    "Engine/ThirdParty/vk-bootstrap/src/**.cpp",
	    
        "Engine/ThirdParty/json/include/**.h",
        "Engine/ThirdParty/json/src/**.cpp",

		"Engine/ThirdParty/ImGuizmo/**.h",
        "Engine/ThirdParty/ImGuizmo/**.cpp",
	
            
	    "Engine/ThirdParty/SPIRV-Reflect/**.h",
	    "Engine/ThirdParty/SPIRV-Reflect/**.c",
	    "Engine/ThirdParty/SPIRV-Reflect/**.cpp",

	    "Engine/ThirdParty/fastgltf/src/**.cpp",
	    "Engine/ThirdParty/fastgltf/deps/simdjson/**.h",
        "Engine/ThirdParty/fastgltf/deps/simdjson/**.cpp",
        
	}


	includedirs
	{ 
		"Engine/Source",
		"Engine/Source/Runtime",
		"Engine/ThirdParty/",
		"%{wks.location}/Intermediates/Reflection/%{prj.name}",
		
	    reflection_directory();
		includedependencies();
	}
	
	links
	 {
	 	"GLFW",
	 	"ImGui",
	 	"EA",
	 	"EnkiTS",
	 	"Tracy",
		"XXHash",
		"RPMalloc",
		"EnTT",
	  	"%{VULKAN_SDK}/lib/vulkan-1.lib",
	 }

	defines
	{
		 "LUMINA_ENGINE_DIRECTORY=%{LuminaEngineDirectory}",
	 	 "LUMINA_ENGINE",
		 "EASTL_USER_DEFINED_ALLOCATOR=1",
		 "GLM_FORCE_DEPTH_ZERO_TO_ONE",
		 "GLFW_INCLUDE_NONE",
		 "GLFW_STATIC",
		 "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
    	 "LUMINA_RENDERER_VULKAN",
    	 "TRACY_ENABLE",
    	 "TRACY_CALLSTACK",
    	 "TRACY_ON_DEMAND",
		 "TRACY_IMPORTS",
	}

	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }


	filter { "system:windows" }
		defines { "LE_PLATFORM_WINDOWS", }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Debug"
	    runtime "Debug"
		links { "%{VULKAN_SDK}/lib/shaderc_combinedd.lib", }
		symbols "On"
		editandcontinue "Off"
		defines { "LE_DEBUG", "_DEBUG", }

	filter "configurations:Release"
		runtime "Release"
		links { "%{VULKAN_SDK}/lib/shaderc_combined.lib", }
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }

	filter "configurations:Shipping"
		links { "%{VULKAN_SDK}/lib/shaderc_combined.lib", }
		runtime "Release"
		optimize "On"
		symbols "Off"
		defines { "LE_SHIP", "NDEBUG", }