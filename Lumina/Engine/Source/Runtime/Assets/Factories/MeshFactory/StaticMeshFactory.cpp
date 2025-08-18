#include "StaticMeshFactory.h"

#include "ImCurveEdit.h"
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
    bool CStaticMeshFactory::DrawImportDialogue(const FString& RawPath, const FString& DestinationPath, bool& bShouldClose)
    {
        bool bShouldImport = false;
        bShouldClose = false;
    
        if (ImGui::BeginTable("GLTFImportTable", 2, ImGuiTableFlags_BordersInnerV))
        {
            ImGui::TableSetupColumn("Option", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.6f);
    
            // Import Materials
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Import Materials");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##ImportMaterials", &Options.bImportMaterials);

            if (Options.bImportMaterials)
            {
                // Import Materials
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Import Textures");
                ImGui::TableSetColumnIndex(1);
                ImGui::Checkbox("##ImportTextures", &Options.bImportTextures);
            }
            
            // Import Animations
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Import Animations");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##ImportAnimations", &Options.bImportAnimations);
    
            // Generate Tangents
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Generate Tangents");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##GenerateTangents", &Options.bGenerateTangents);
    
            // Merge Meshes
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Merge Meshes");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##MergeMeshes", &Options.bMergeMeshes);
    
            // Apply Transforms
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Apply Transforms");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##ApplyTransforms", &Options.bApplyTransforms);
    
            // Mesh Compression
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Use Mesh Compression");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##MeshCompression", &Options.bUseCompression);
    
            // Import Scale
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Scale");
            ImGui::TableSetColumnIndex(1);
            ImGui::DragFloat("##ImportScale", &Options.Scale, 0.01f, 0.01f, 100.0f, "%.2f");
    
            ImGui::EndTable();
        }
    
        if (ImGui::Button("Import"))
        {
            bShouldImport = true;
            bShouldClose = true;
        }
    
        ImGui::SameLine();
    
        if (ImGui::Button("Cancel"))
        {
            bShouldImport = false;
            bShouldClose = true;
        }
    
        return bShouldImport;
    }


    void CStaticMeshFactory::TryImport(const FString& RawPath, const FString& DestinationPath)
    {
        FString VirtualPath = Paths::ConvertToVirtualPath(DestinationPath);

        Import::Mesh::GLTF::FGLTFImportData ImportData;
        Import::Mesh::GLTF::ImportGLTF(ImportData, Options, RawPath);

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

            FTaskSystem::Get().ParallelFor((uint32)ImportData.Textures.size(), [&](uint32 Index)
            {
                CTextureFactory* TextureFactory = CTextureFactory::StaticClass()->GetDefaultObject<CTextureFactory>();

                const Import::Mesh::GLTF::FGLTFImage& Image = ImportData.Textures[Index];
                FString ParentPath = Paths::Parent(RawPath);
                FString TexturePath = ParentPath + "/" + Image.RelativePath;
                FString TextureFileName = Paths::RemoveExtension(Paths::FileName(TexturePath));
                                         
                FString DestinationParent = Paths::Parent(FullPath);
                FString TextureDestination = DestinationParent + "/" + TextureFileName;
                                             
                TextureFactory->TryImport(TexturePath, TextureDestination);
            });

            for (SIZE_T i = 0; i < ImportData.Materials[Counter].size(); ++i)
            {
                const Import::Mesh::GLTF::FGLTFMaterial& Material = ImportData.Materials[Counter][i];
                FName MaterialName = (i == 0) ? FString(FileName + "_Material").c_str() : FString(FileName + "_Material" + eastl::to_string(i)).c_str();
                //CMaterial* NewMaterial = NewObject<CMaterial>(NewPackage, MaterialName.c_str());
                NewMesh->Materials.push_back(nullptr);
            }
            NewMesh->SetFlag(OF_NeedsLoad);
            
            CPackage::SavePackage(NewPackage, NewMesh, FullPath.c_str());
            NewPackage->LoadObject(NewMesh);

            Counter++;
        }

        Options = {};
    }
}
