#pragma once

#include "Assets/Factories/Factory.h"
#include "StaticMeshFactory.generated.h"



namespace Lumina
{
    LUM_CLASS()
    class CStaticMeshFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CObject* CreateNew(const FString& Path) override;
        FString GetAssetName() const override { return "Static Mesh"; }

        void CreateAssetFile(const FString& Path) override;
    };
}
