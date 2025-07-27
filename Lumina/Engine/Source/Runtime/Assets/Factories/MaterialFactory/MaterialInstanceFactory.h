#pragma once
#include "Assets/AssetTypes/Material/MaterialInstance.h"
#include "Assets/Factories/Factory.h"
#include "MaterialInstanceFactory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CMaterialInstanceFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CClass* GetSupportedType() const override { return CMaterialInstance::StaticClass(); }
        FString GetAssetName() const override { return "Material Instance"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewMaterialInstance"; }

        bool HasCreationDialogue() const override;
        bool DrawCreationDialogue(const FString& Path, bool& bShouldClose) override;
        CObject* CreateNew(const FName& Name, CPackage* Package) override;

    private:

        CMaterial* SelectedMaterial = nullptr;
    };
}
