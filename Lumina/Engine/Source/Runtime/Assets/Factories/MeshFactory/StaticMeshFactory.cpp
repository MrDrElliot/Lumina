#include "StaticMeshFactory.h"
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include "Log/Log.h"
#include "Renderer/RenderTypes.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"



namespace Lumina
{
    std::shared_ptr<LAsset> FStaticMeshFactory::CreateNew(const FAssetMetadata& Metadata, FArchive& Archive)
    {
        auto NewMesh = std::make_shared<LStaticMesh>();
        NewMesh->Serialize(Archive);
        NewMesh->CreateNew();

        return NewMesh;
    }

    TFastVector<std::shared_ptr<LStaticMesh>> FStaticMeshFactory::Import(std::filesystem::path InPath)
    {
        fastgltf::Asset Asset;

        TFastVector<std::shared_ptr<LStaticMesh>> ReturnMeshes;
        
        ExtractAsset(&Asset, InPath);
        
        ReturnMeshes.reserve((uint32)Asset.meshes.size());
        for (fastgltf::Mesh& Mesh : Asset.meshes)
        {
            FMeshAsset NewAsset;
            //TFastVector<uint32> Indices;
           // TFastVector<FVertex> Vertices;

            for (auto& Primitive : Mesh.primitives)
            {
                /*FMeshSurface NewSurface;
                NewSurface.StartIndex = Indices.size();
                NewSurface.Count = Asset.accessors[Primitive.indicesAccessor.value()].count;*/

                size_t IniitalVert = NewAsset.Vertices.size();

                fastgltf::Accessor& IndexAccessor = Asset.accessors[Primitive.indicesAccessor.value()];
                NewAsset.Indices.reserve(NewAsset.Indices.size() + IndexAccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(Asset, IndexAccessor, [&](std::uint32_t Index)
                {
                    NewAsset.Indices.push_back(Index);
                });

                fastgltf::Accessor& PosAccessor = Asset.accessors[Primitive.findAttribute("POSITION")->second];
                NewAsset.Vertices.resize(NewAsset.Vertices.size() + PosAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, PosAccessor, [&](glm::vec3 V, size_t Index)
                {
                    FVertex Vertex;
                    Vertex.Position = V;
                    Vertex.Color = glm::vec4(1.0f);
                    Vertex.UV = glm::vec2(0.0f);
                    
                    NewAsset.Vertices[IniitalVert + Index] = Vertex;
                });

                auto normals = Primitive.findAttribute("NORMAL");
                if (normals != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, Asset.accessors[(*normals).second], [&](glm::vec3 v, size_t index)
                    {
                            NewAsset.Vertices[IniitalVert + index].Normal = v;
                    });
                }

                auto uv = Primitive.findAttribute("TEXCOORD_0");
                if (uv != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(Asset, Asset.accessors[(*uv).second], [&](glm::vec2 v, size_t index)
                    {
                            NewAsset.Vertices[IniitalVert + index].UV.x = v.x;
                            NewAsset.Vertices[IniitalVert + index].UV.y = 1.0f - v.y;
                    });
                }

                // load vertex colors
                auto colors = Primitive.findAttribute("COLOR_0");
                if (colors != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(Asset, Asset.accessors[(*colors).second], [&](glm::vec4 v, size_t index)
                    {
                            NewAsset.Vertices[IniitalVert + index].Color = v;
                    });
                }
                
                NewAsset.Name = Mesh.name;
            }

            std::random_device rd;  // Obtain a random number from hardware
            std::mt19937 gen(rd()); // Seed the generator
            std::uniform_real_distribution<> dis(0.0, 1.0); 
            constexpr bool OverrideColors = true;
            if (OverrideColors)
            {
                for (FVertex& vtx : NewAsset.Vertices)
                {
                    float r = dis(gen);  // Generate a random red component
                    float g = dis(gen);  // Generate a random green component
                    float b = dis(gen);  // Generate a random blue component
                    vtx.Color = glm::vec4(r, g, b, 1.0f);  // Set the vertex color
                }
            }
            
            ReturnMeshes.push_back(LStaticMesh::CreateMesh(FAssetMetadata(), std::move(NewAsset)));
        }

        return ReturnMeshes;
    }


    void FStaticMeshFactory::ExtractAsset(fastgltf::Asset* OutAsset, std::filesystem::path InPath)
    {
        // Allocate crucial fastgltf objects
        fastgltf::Parser gltf_parser;
        fastgltf::GltfDataBuffer data_buffer;

        // Try to load asset data
        if (!data_buffer.loadFromFile(InPath))
        {
            LOG_ERROR("Failed to load glTF model with path: {0}. Aborting import.", InPath.string());
            return;
        }

        fastgltf::GltfType source_type = fastgltf::determineGltfFileType(&data_buffer);

        if (source_type == fastgltf::GltfType::Invalid)
        {
            LOG_ERROR("Failed to determine glTF file type with path: {0}. Aborting import.", InPath.string());
            return;
        }

        constexpr fastgltf::Options options = fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices;

        fastgltf::Expected<fastgltf::Asset> expected_asset(fastgltf::Error::None);

        source_type == fastgltf::GltfType::glTF ? expected_asset = gltf_parser.loadGltf(&data_buffer, InPath.parent_path(), options) :
            expected_asset = gltf_parser.loadGltfBinary(&data_buffer, InPath.parent_path(), options);

        if (const auto error = expected_asset.error(); error != fastgltf::Error::None)
        {
            LOG_ERROR("Failed to load asset source with path: {0}. [{1}]: {2} Aborting import.", InPath.string(),
            fastgltf::getErrorName(error), fastgltf::getErrorMessage(error));
        }

        *OutAsset = std::move(expected_asset.get());
    }
    
}