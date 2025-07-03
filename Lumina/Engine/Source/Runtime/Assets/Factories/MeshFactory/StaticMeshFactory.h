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

        FString GetAssetName() const override { return "Static Mesh"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewMesh"; }

        void TryImport(const FString& RawPath, const FString& DestinationPath) override;
    };
}
