#pragma once
#include "Assets/Definition/AssetDefinition.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "AssetDefinition_StaticMesh.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CAssetDefinition_StaticMesh : public CAssetDefinition
    {
        GENERATED_BODY()
        
    public:

        FString GetAssetDisplayName() const override { return "Static Mesh"; }
        FString GetAssetDescription() const override { return "A static mesh."; }
        CClass* GetAssetClass() const override { return CStaticMesh::StaticClass(); }
        CFactory* GetFactory() const override { return GetMutableDefault<CStaticMeshFactory>(); }
        bool CanImport() override { return true; }
        bool IsExtensionSupported(const FString& Ext) override { return Ext == ".gltf"; }
    
    };
}
