#pragma once
#include "Assets/AssetTypes/Material/MaterialInstance.h"
#include "Assets/Definition/AssetDefinition.h"
#include "Assets/Factories/MaterialFactory/MaterialInstanceFactory.h"
#include "Core/Object/ObjectMacros.h"
#include "AssetDefinition_MaterialInstance.generated.h"
namespace Lumina
{
    LUM_CLASS()
    class CAssetDefinition_MaterialInstance : public CAssetDefinition
    {
        GENERATED_BODY()
    public:

        
        FString GetAssetDisplayName() const override { return "Material Instance"; }
        FString GetAssetDescription() const override { return "An instance of a material."; }
        CClass* GetAssetClass() const override { return CMaterialInstance::StaticClass(); }
        CFactory* GetFactory() const override { return GetMutableDefault<CMaterialInstanceFactory>(); }
        
    
    };
}
