#pragma once
#include "Assets/AssetTypes/Archetype/Archetype.h"
#include "Assets/Definition/AssetDefinition.h"
#include "Assets/Factories/ArchetypeFactory/ArchetypeFactory.h"
#include "assetdefinition_archetype.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CAssetDefinition_Archetype: public CAssetDefinition
    {
        GENERATED_BODY()
    public:

        FString GetAssetDisplayName() const override { return "Archetype"; }
        FString GetAssetDescription() const override { return "An archetype."; }
        CClass* GetAssetClass() const override { return CArchetype::StaticClass(); }
        CFactory* GetFactory() const override { return GetMutableDefault<CArchetypeFactory>(); }
        
    };
}
