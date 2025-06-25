#pragma once

#include "Core/Assertions/Assert.h"
#include "Core/Serialization/Archiver.h"

namespace Lumina
{

	class AssetImporter
	{
		
	public:
		
		virtual bool Import(FArchive& Ar, void* ImportData, const FString& AssetPath) { LUMINA_NO_ENTRY(); }
		

	};

}



