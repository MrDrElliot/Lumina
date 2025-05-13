#pragma once

#include "Assets/Factories/Factory.h"

namespace Lumina
{
	class FMaterialFactory : public FFactory
	{
	public:

		FAssetPath CreateNew(const FString& Path) override;
		const FString& GetAssetName() const override { return "Material"; }

	};
}