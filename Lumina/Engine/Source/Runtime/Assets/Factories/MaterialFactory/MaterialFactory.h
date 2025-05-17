#pragma once

#include "Assets/Factories/Factory.h"
#include "MaterialFactory.generated.h"

namespace Lumina
{
	LUM_CLASS()
	class CMaterialFactory : public CFactory
	{
		GENERATED_BODY()
	public:

		CObject* CreateNew(const FString& Path) override;
		FString GetAssetName() const override { return "Material"; }

		void CreateAssetFile(const FString& Path) override;
	};
}