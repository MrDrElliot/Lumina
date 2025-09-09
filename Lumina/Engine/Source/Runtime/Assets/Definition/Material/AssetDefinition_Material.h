#pragma once

#include "Assets/Definition/AssetDefinition.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/Factories/MaterialFactory/MaterialFactory.h"
#include "AssetDefinition_Material.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CAssetDefinition_Material : public CAssetDefinition
    {
        GENERATED_BODY()
    public:

        FString GetAssetDisplayName() const override { return "Material"; }
        FString GetAssetDescription() const override { return "A material."; }
        CClass* GetAssetClass() const override { return CMaterial::StaticClass(); }
        CFactory* GetFactory() const override { return GetMutableDefault<CMaterialFactory>(); }
        
    };
}
