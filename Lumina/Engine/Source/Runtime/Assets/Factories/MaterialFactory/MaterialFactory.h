#pragma once

#include "Assets/Factories/Factory.h"

namespace Lumina
{
	class FMaterialFactory : public FFactory
	{
	public:

		ELoadResult LoadFromDisk(FAssetRecord* InRecord) override;
		IAsset* CreateNew(const FString& Path) override;
		const FString& GetAssetName() const override { return "Material"; }

	};
}