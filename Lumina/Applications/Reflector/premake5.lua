include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Reflector"
	kind "ConsoleApp"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")

    configurations { "Release" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
	removedefines { }

	postbuildcommands 
	{
	    "{COPYFILE} \"%{wks.location}/External/LLVM/bin/libclang.dll\" \"%{wks.location}/Binaries/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/\"",
	}


	libdirs
	{
		"%{LuminaEngineDirectory}/External/LLVM/Lib",
		"%{LuminaEngineDirectory}/External/LLVM/bin",
	}

	links
	{
	  	"EA",
	  	
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
		"%{wks.location}/Lumina/Engine/ThirdParty/xxhash/**.h",
		"%{wks.location}/Lumina/Engine/ThirdParty/xxhash/**.c",
	}


	includedirs
	{ 
		"Source",
	    
	   	"%{LuminaEngineDirectory}/Lumina",
		"%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/",
		"%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EA/EABase/include/common",
		"%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EA/EASTL/include/",
		"%{LuminaEngineDirectory}/External/LLVM/include/",
	    "%{wks.location}/Intermediates/Reflection/Reflector/",
	    
	    reflection_directory();
		includedependencies();
		
	}

