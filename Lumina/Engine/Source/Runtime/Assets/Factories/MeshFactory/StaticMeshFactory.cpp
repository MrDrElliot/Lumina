#include "StaticMeshFactory.h"

#include <random>
#include <glm/glm.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/base64.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include "Assets/AssetHeader.h"
#include "Renderer/RHIIncl.h"
#include "Log/Log.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Object/Package/Package.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Core/Serialization/Package/PackageSaver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"


namespace Lumina
{

    static void ExtractAsset(fastgltf::Asset* OutAsset, const FString& InPath)
    {
        std::filesystem::path FSPath = InPath.c_str();
        
        // Allocate crucial fastgltf objects
        fastgltf::Parser gltf_parser;
        fastgltf::GltfDataBuffer data_buffer;

        // Try to load asset data
        if (!data_buffer.loadFromFile(FSPath))
        {
            LOG_ERROR("Failed to load glTF model with path: {0}. Aborting import.", FSPath.string());
            return;
        }

        fastgltf::GltfType source_type = fastgltf::determineGltfFileType(&data_buffer);

        if (source_type == fastgltf::GltfType::Invalid)
        {
            LOG_ERROR("Failed to determine glTF file type with path: {0}. Aborting import.", FSPath.string());
            return;
        }

        constexpr fastgltf::Options options = fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices;

        fastgltf::Expected<fastgltf::Asset> expected_asset(fastgltf::Error::None);

        source_type == fastgltf::GltfType::glTF ? expected_asset = gltf_parser.loadGltf(&data_buffer, FSPath.parent_path(), options) :
            expected_asset = gltf_parser.loadGltfBinary(&data_buffer, FSPath.parent_path(), options);

        if (const auto error = expected_asset.error(); error != fastgltf::Error::None)
        {
            LOG_ERROR("Failed to load asset source with path: {0}. [{1}]: {2} Aborting import.", FSPath.string(),
            fastgltf::getErrorName(error), fastgltf::getErrorMessage(error));
        }

        *OutAsset = std::move(expected_asset.get());
    }
    

    void CStaticMeshFactory::TryImport(const FString& RawPath, const FString& DestinationPath)
    {
        FString FullPath = DestinationPath + ".lasset";
        FString VirtualPath = Paths::ConvertToVirtualPath(DestinationPath);
        
        fastgltf::Asset Asset;
        ExtractAsset(&Asset, RawPath);

        CPackage* NewPackage = CPackage::LoadPackage(VirtualPath.c_str());
        CStaticMesh* Temp = NewObject<CStaticMesh>(NewPackage);
        
        //CreateAssetFile(DestinationPath);
        
        for (fastgltf::Mesh& Mesh : Asset.meshes)
        {
            FMeshResource NewAsset;
        
            for (auto& Primitive : Mesh.primitives)
            {
                size_t InitialVert = NewAsset.Vertices.size();
        
                fastgltf::Accessor& IndexAccessor = Asset.accessors[Primitive.indicesAccessor.value()];
                NewAsset.Indices.reserve(IndexAccessor.count);
        
                fastgltf::iterateAccessor<std::uint32_t>(Asset, IndexAccessor, [&](std::uint32_t Index)
                {
                    NewAsset.Indices.push_back(Index);
                });
        
                fastgltf::Accessor& PosAccessor = Asset.accessors[Primitive.findAttribute("POSITION")->second];
                NewAsset.Vertices.resize(NewAsset.Vertices.size() + PosAccessor.count);
        
                fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, PosAccessor, [&](glm::vec3 V, size_t Index)
                {
                    FVertex Vertex;
                    Vertex.Position.x = V.x;
                    Vertex.Position.y = V.y;
                    Vertex.Position.z = V.z;
                    Vertex.Position.w = 1.0f;
                    
                    NewAsset.Vertices[InitialVert + Index] = Memory::Move(Vertex);
                });
        
                auto normals = Primitive.findAttribute("NORMAL");
                if (normals != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, Asset.accessors[(*normals).second], [&](glm::vec3 v, size_t index)
                    {
                        NewAsset.Vertices[InitialVert + index].Normal.x = v.x;
                        NewAsset.Vertices[InitialVert + index].Normal.y = v.y;
                        NewAsset.Vertices[InitialVert + index].Normal.z = v.y;
                        NewAsset.Vertices[InitialVert + index].Normal.w = 1.0f;

                    });
                }
        
                auto uv = Primitive.findAttribute("TEXCOORD_0");
                if (uv != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(Asset, Asset.accessors[(*uv).second], [&](glm::vec2 v, size_t index)
                    {
                        NewAsset.Vertices[InitialVert + index].UV.x = v.x;
                        NewAsset.Vertices[InitialVert + index].UV.y = 1.0f - v.y;
                    });
                }
        
                // load vertex colors
                auto colors = Primitive.findAttribute("COLOR_0");
                if (colors != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(Asset, Asset.accessors[(*colors).second], [&](glm::vec4 v, size_t index)
                    {
                        NewAsset.Vertices[InitialVert + index].Color.r = v.r;
                        NewAsset.Vertices[InitialVert + index].Color.g = v.g;
                        NewAsset.Vertices[InitialVert + index].Color.b = v.b;
                        NewAsset.Vertices[InitialVert + index].Color.a = v.a;
                    });
                }
                
            }
            
            constexpr bool OverrideColors = true;
            if (OverrideColors)
            {
                std::random_device rd;
                std::mt19937 gen(rd()); // Standard Mersenne Twister
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);

                for (FVertex& vtx : NewAsset.Vertices)
                {
                    vtx.Color = glm::vec4(dist(gen), dist(gen), dist(gen), 1.0f);
                }
            }

            Temp->MeshResource = Memory::Move(NewAsset);

            FAssetHeader Header;
            Header.Path = VirtualPath;
            Header.ClassName = "CStaticMesh";
            Header.Type = EAssetType::StaticMesh;
            Header.Version = 1;

            TVector<uint8> Buffer;
            FPackageSaver Saver(Buffer, nullptr);
            Saver << Header;
        
            FBinaryStructuredArchive BinaryAr(Saver);
            Temp->Serialize(BinaryAr.Open());
            
            FileHelper::SaveArrayToFile(Buffer, FullPath);
        
            Temp->DestroyNow();
        }
    }
}
