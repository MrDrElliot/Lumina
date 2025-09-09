#pragma once
#include "Assets/AssetTypes/Archetype/Archetype.h"
#include "assets/factories/factory.h"
#include "Core/Object/ObjectMacros.h"
#include "archetypefactory.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CArchetypeFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CClass* GetSupportedType() const override { return CArchetype::StaticClass(); }
        FString GetAssetName() const override { return "Archetype"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewArchetype"; }

        CObject* CreateNew(const FName& Name, CPackage* Package) override;
    };
}
