#include "StaticMeshFactory.h"

#include "Tools/Import/ImportHelpers.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "TaskSystem/TaskSystem.h"


namespace Lumina
{
    void CStaticMeshFactory::TryImport(const FString& RawPath, const FString& DestinationPath)
    {
        FString VirtualPath = Paths::ConvertToVirtualPath(DestinationPath);

        Import::Mesh::GLTF::FGLTFImportData ImportData;
        Import::Mesh::GLTF::ImportGLTF(ImportData, RawPath);

        uint32 Counter = 0;
        for (const FMeshResource& MeshResource : ImportData.Resources)
        {
            FString QualifiedPath = DestinationPath + eastl::to_string(Counter);
            FString FileName = Paths::FileName(QualifiedPath, true);
            
            FString FullPath = QualifiedPath;
            Paths::AddPackageExtension(FullPath);
            
            CPackage* NewPackage = CPackage::CreatePackage(GetSupportedType()->GetName().ToString(), QualifiedPath);
            CStaticMesh* NewMesh = NewObject<CStaticMesh>(NewPackage, FileName.c_str());
            NewMesh->MeshResources = MeshResource;
            
            FLambdaTask* Task = FTaskSystem::Get()->ScheduleLambda((uint32)ImportData.Textures.size(), [ImportData, RawPath, FullPath] (uint32 Start, uint32 End, uint32 Thread)
            {
                CTextureFactory* TextureFactory = CTextureFactory::StaticClass()->GetDefaultObject<CTextureFactory>();
                
                for(uint32 i = Start; i < End; ++i)
                {
                    const Import::Mesh::GLTF::FGLTFImage& Image = ImportData.Textures[i];
                    FString ParentPath = Paths::Parent(RawPath);
                    FString TexturePath = ParentPath + "/" + Image.RelativePath;
                    FString TextureFileName = Paths::RemoveExtension(Paths::FileName(TexturePath));
                                             
                    FString DestinationParent = Paths::Parent(FullPath);
                    FString TextureDestination = DestinationParent + "/" + TextureFileName;
                                                 
                    TextureFactory->TryImport(TexturePath, TextureDestination);
                }
            });

            FTaskSystem::Get()->WaitForTask(Task);
            
            for (SIZE_T i = 0; i < ImportData.Materials[Counter].size(); ++i)
            {
                const Import::Mesh::GLTF::FGLTFMaterial& Material = ImportData.Materials[Counter][i];
                FName MaterialName = FString(FileName + "_Material" + eastl::to_string(i)).c_str();
                //CMaterial* NewMaterial = NewObject<CMaterial>(NewPackage, MaterialName.c_str());
                NewMesh->Materials.push_back(nullptr);
            }
            NewMesh->SetFlag(OF_NeedsLoad);
            
            CPackage::SavePackage(NewPackage, NewMesh, FullPath.c_str());
            NewPackage->LoadObject(NewMesh);

            Counter++;
        }
    }
}
