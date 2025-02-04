#pragma once

#include "Assets/Factories/Factory.h"

namespace Lumina
{
	class FMaterialFactory : public FFactory
	{
	public:

		ELoadResult CreateNew(FAssetRecord* InRecord) override;

	};
}