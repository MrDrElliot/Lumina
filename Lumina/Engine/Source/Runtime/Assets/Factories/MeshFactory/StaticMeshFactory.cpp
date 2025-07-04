#include "StaticMeshFactory.h"

#include "Tools/Import/ImportHelpers.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"


namespace Lumina
{
    void CStaticMeshFactory::TryImport(const FString& RawPath, const FString& DestinationPath)
    {
        FString FullPath = DestinationPath;
        Paths::AddPackageExtension(FullPath);
        FString VirtualPath = Paths::ConvertToVirtualPath(DestinationPath);
        FString FileName = Paths::FileName(DestinationPath, true);
        
        CPackage* NewPackage = CPackage::CreatePackage(GetSupportedType()->GetName().ToString(), DestinationPath);
        CStaticMesh* NewMesh = NewObject<CStaticMesh>(NewPackage, FileName.c_str());
        NewMesh->SetFlag(OF_NeedsLoad);
        
        ImportHelpers::GetMeshVertexData(NewMesh->MeshResource.Vertices, NewMesh->MeshResource.Indices, RawPath);

        CPackage::SavePackage(NewPackage, NewMesh, FullPath.c_str());
        NewPackage->LoadObject(NewMesh);
    }
}
