include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Lumina"
	kind "SharedLib"
		
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{LuminaEngineDirectory}/Binaries/" .. outputdir)
    objdir ("%{LuminaEngineDirectory}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")    
	
	prebuildcommands
    {
        "\"%{LuminaEngineDirectory}/Binaries/Release-windows-x86_64/Reflector.exe\" \"%{wks.location}%{wks.name}.sln\" && echo Reflection completed."
    }

	postbuildcommands
	{
		'{COPYFILE} "%{LuminaEngineDirectory}/External/PhysX/physx/bin/win.x86_64.vc143.mt/checked/*.dll" "%{cfg.targetdir}"',
	}
    	
	files
	{
		"Engine/Source/**.h",
		"Engine/Source/**.cpp",
		
		"%{LuminaEngineDirectory}/Intermediates/Reflection/Lumina/**.h",
		"%{LuminaEngineDirectory}/Intermediates/Reflection/Lumina/**.cpp",
		
		"Engine/ThirdParty/stb_image/**.h",

		"Engine/ThirdParty/glm/glm/**.hpp",
		"Engine/ThirdParty/glm/glm/**.cpp",
		"Engine/ThirdParty/glm/glm/**.inl",

		"Engine/ThirdParty/imgui/imgui_demo.cpp",
		
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
		"%{LuminaEngineDirectory}/Intermediates/Reflection/%{prj.name}",
		
	    reflection_directory();
		includedependencies();
	}
	
	libdirs
	{
		"%{VULKAN_SDK}/lib",
		"%{LuminaEngineDirectory}/External/PhysX/physx/bin/win.x86_64.vc143.mt/checked/",
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
	  	"vulkan-1.lib",
		"PhysXExtensions_static_64.lib",
		"PhysX_64.lib",
		"PhysXPvdSDK_static_64.lib",
		"PhysXCommon_64.lib",
		"PhysXFoundation_64.lib",
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