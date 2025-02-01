#pragma once

#include "Core/Serialization/Archiver.h"

namespace Lumina
{

	class AssetImporter
	{
	public:
		
		virtual bool Import(FArchive& Ar, void* ImportData, const FString& AssetPath) = 0;
	
	private:


	};

}



