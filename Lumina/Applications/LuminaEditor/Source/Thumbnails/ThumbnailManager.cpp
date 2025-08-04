#include "ThumbnailManager.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Object/Package/Thumbnail/ThumbnailGenerator.h"
#include "Scene/ScenePrimitives.h"


namespace Lumina
{

    CThumbnailManager* CThumbnailManager::ThumbnailManagerSingleton = nullptr;

    CThumbnailManager::CThumbnailManager()
    {
    }

    void CThumbnailManager::Initialize()
    {
        CubeMesh = NewObject<CStaticMesh>();

        TVector<FVertex> Vertices;
        TVector<uint32> Indices;
        Primitives::GenerateCube(Vertices, Indices);
        
        FMeshResource Resource;
        Resource.Vertices = Vertices;
        Resource.Indices = Indices;
        
        FGeometrySurface Surface;
        Surface.ID = "Mesh";
        Surface.IndexCount = Indices.size();
        Surface.StartIndex = 0;
        Surface.MaterialIndex = 0;
        Resource.GeometrySurfaces.push_back(Surface);

        CubeMesh->Materials.resize(1);
        CubeMesh->SetMeshResource(Resource);
    }

    CThumbnailManager& CThumbnailManager::Get()
    {
        if (ThumbnailManagerSingleton == nullptr)
        {
            ThumbnailManagerSingleton = NewObject<CThumbnailManager>();
            ThumbnailManagerSingleton->Initialize();
        }
        
        return *ThumbnailManagerSingleton;
    }

    void CThumbnailManager::GetOrLoadThumbnailsForPackages(TSpan<FString> Packages)
    {
        for (const FString& PackagePath : Packages)
        {
            FName PackageName = PackagePath;
            
        }
    }
}
