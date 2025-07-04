#pragma once

#include "Assets/Factories/Factory.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "StaticMeshFactory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CStaticMeshFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CClass* GetSupportedType() const override { return CStaticMesh::StaticClass(); }
        FString GetAssetName() const override { return "Static Mesh"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewMesh"; }

        void TryImport(const FString& RawPath, const FString& DestinationPath) override;
    };
}
