#pragma once
#include "Assets/Definition/AssetDefinition.h"
#include "assets/factories/worldfactory/worldfactory.h"
#include "World/World.h"
#include "AssetDefinition_World.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CAssetDefinition_World : public CAssetDefinition
    {
        GENERATED_BODY()
    public:

        CFactory* GetFactory() const override { return GetMutableDefault<CWorldFactory>(); }
        CClass* GetAssetClass() const override { return CWorld::StaticClass(); }
        FString GetAssetDisplayName() const override { return "World"; }
    
    };
}
