#pragma once
#include "Platform/GenericPlatform.h"

#define PACKAGE_FILE_TAG			0x9E2A83C1

#define PREPROCESSOR_ENUM_PROTECT(a) ((unsigned int)(a))

enum class ELuminaEngineVersion : uint32
{
	INITIAL_VERSION = 1000,


	AUTOMATIC_VERSION_PLUS_ONE,
	AUTOMATIC_VERSION = AUTOMATIC_VERSION_PLUS_ONE - 1
};


struct FPackageFileVersion
{

	FPackageFileVersion(ELuminaEngineVersion EngineVersion)
	: FileVersion(static_cast<int32>(EngineVersion)) {}
	
	/** UE5 version comparisons */
	bool operator >=(ELuminaEngineVersion Version) const
	{
		return FileVersion >= static_cast<int32>(Version);
	}


public:

	int32		FileVersion = 0;
};

#define VER_LATEST_ENGINE           PREPROCESSOR_ENUM_PROTECT(ELuminaEngineVersion::AUTOMATIC_VERSION)

extern const FPackageFileVersion GPackageFileLuminaVersion;

