#pragma once
#include "World/World.h"
#include "Assets/Factories/Factory.h"
#include "core/object/ObjectMacros.h"
#include "WorldFactory.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CWorldFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CClass* GetSupportedType() const override { return CWorld::StaticClass(); }
        FString GetAssetName() const override { return "World"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewWorld"; }

        CObject* CreateNew(const FName& Name, CPackage* Package) override;
    
    };
}
