#pragma once

#include "Assets/Factories/Factory.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Tools/Import/ImportHelpers.h"
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

        bool HasImportDialogue() const override { return true; }
        bool DrawImportDialogue(const FString& RawPath, const FString& DestinationPath, bool& bShouldClose) override;
        void TryImport(const FString& RawPath, const FString& DestinationPath) override;

    private:

        Import::Mesh::GLTF::FGLTFImportOptions Options;

    };
}
