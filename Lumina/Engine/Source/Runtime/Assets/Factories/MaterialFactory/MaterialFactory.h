#pragma once

#include "Assets/Factories/Factory.h"

namespace Lumina
{
	class FMaterialFactory : public FFactory
	{
	public:

		ELoadResult CreateNew(const FAssetHandle& InHandle, const FAssetPath& InPath, FAssetRecord* InRecord, FArchive& Archive) override;

	};
}