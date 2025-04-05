include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Reflector"
	kind "ConsoleApp"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediates/" .. outputdir .. "/%{prj.name}")
		
	removedefines { }
	
    postbuildcommands 
    {
        "{COPY} \"%{wks.location}/External/LLVM/bin/libclang.dll\" \"%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}\""
    }


	
	libdirs
	{ 
		"%{LuminaEngineDirectory}/External/LLVM/Lib",
		"%{LuminaEngineDirectory}/External/LLVM/bin",
		"$(VULKAN_SDK)/lib/"
	}
		
	links
	 {
		"Lumina",
	  	"GLFW",
	  	"imgui",
	  	"vulkan-1.lib",
	  	"clangBasic.lib",
	  	"clangLex.lib",
	  	"clangAST.lib",
	  	"libclang.lib",
	  	"LLVMAnalysis.lib",
	  	"LLVMBinaryFormat.lib",
	  	"LLVMBitReader.lib",
	  	"LLVMBitstreamReader.lib",
	  	"LLVMDemangle.lib",
	  	"LLVMFrontendOffloading.lib",
	  	"LLVMFrontendOpenMP.lib",
	  	"LLVMMC.lib",
	  	"LLVMProfileData.lib",
	  	"LLVMRemarks.lib",
	  	"LLVMScalarOpts.lib",
	  	"LLVMTargetParser.lib",
	  	"LLVMTransformUtils.lib",
	  	"LLVMCore.lib",
        "LLVMSupport.lib",
	 }
	  

	files
	{
		"Source/**.h",
		"Source/**.cpp",

	}


	includedirs
	{ 
		"Source",
	    
	   	"%{LuminaEngineDirectory}/Lumina",
		"%{LuminaEngineDirectory}/Lumina/Engine/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/Runtime/",
	    "%{LuminaEngineDirectory}/External/LLVM/include/",
	    
	    reflection_directory();
		includedependencies();
		
	}


	filter "system:linux"
		defines { "LE_PLATFORM_LINUX" }

	filter { "system:windows" }	
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		optimize "On"
		defines { "LE_RELEASE", "NDEBUG", }
