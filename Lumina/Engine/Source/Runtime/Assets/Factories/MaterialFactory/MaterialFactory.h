#pragma once

#include "Assets/Factories/Factory.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "MaterialFactory.generated.h"


namespace Lumina
{
	LUM_CLASS()
	class CMaterialFactory : public CFactory
	{
		GENERATED_BODY()
	public:


		CClass* GetSupportedType() const override { return CMaterial::StaticClass(); }
		FString GetAssetName() const override { return "Material"; }
		FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewMaterial"; }

		CObject* CreateNew(const FName& Name, CPackage* Package) override;
	};
}
