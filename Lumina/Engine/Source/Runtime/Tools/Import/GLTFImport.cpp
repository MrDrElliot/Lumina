#include "ImportHelpers.h"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <glm/glm.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/base64.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include "Memory/Memory.h"
#include "Paths/Paths.h"
#include "Renderer/MeshData.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderResource.h"
#include "Renderer/Vertex.h"

namespace Lumina::Import::Mesh::GLTF
{
    namespace
    {
        void ExtractAsset(fastgltf::Asset* OutAsset, const FString& InPath)
        {
            std::filesystem::path FSPath = InPath.c_str();
        
            fastgltf::Parser gltf_parser;
            fastgltf::GltfDataBuffer data_buffer;

            if (!data_buffer.loadFromFile(FSPath))
            {
                LOG_ERROR("Failed to load glTF model with path: {0}. Aborting import.", FSPath.string());
                return;
            }

            fastgltf::GltfType SourceType = fastgltf::determineGltfFileType(&data_buffer);

            if (SourceType == fastgltf::GltfType::Invalid)
            {
                LOG_ERROR("Failed to determine glTF file type with path: {0}. Aborting import.", FSPath.string());
                return;
            }

            constexpr fastgltf::Options options = fastgltf::Options::DontRequireValidAssetMember |
                fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers| fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices;

            fastgltf::Expected<fastgltf::Asset> expected_asset(fastgltf::Error::None);

            if (SourceType == fastgltf::GltfType::glTF)
            {
                expected_asset = gltf_parser.loadGltf(&data_buffer, FSPath.parent_path(), options);
            }
            else if (SourceType == fastgltf::GltfType::GLB)
            {
                expected_asset = gltf_parser.loadGltfBinary(&data_buffer, FSPath.parent_path(), options);
            }
            else
            {
                LOG_ERROR("GLTF Source Type Invalid");
                return;
            }

            if (const auto error = expected_asset.error(); error != fastgltf::Error::None)
            {
                LOG_ERROR("Failed to load asset source with path: {0}. [{1}]: {2} Aborting import.", FSPath.string(),
                fastgltf::getErrorName(error), fastgltf::getErrorMessage(error));
            }

            *OutAsset = std::move(expected_asset.get());
        }
    }
    

    void ImportGLTF(FGLTFImportData& OutData, const FString& RawFilePath)
    {
        fastgltf::Asset Asset;
        ExtractAsset(&Asset, RawFilePath);
        FString Name = Paths::FileName(RawFilePath, true);
        
        OutData.Resources.clear();
        OutData.Resources.reserve(Asset.meshes.size());

        for (fastgltf::Mesh& Mesh : Asset.meshes)
        {
            FMeshResource NewResource;
            NewResource.GeometrySurfaces.reserve(100);
            NewResource.Name = Mesh.name.empty() ? FName(Name + "_" + eastl::to_string(OutData.Resources.size())) : Mesh.name.c_str();

            SIZE_T IndexCount = 0;

            for (auto& Material : Asset.materials)
            {
                FGLTFMaterial NewMaterial;
                NewMaterial.Name = Material.name.c_str();
                
                OutData.Materials[OutData.Resources.size()].push_back(NewMaterial);
            }

            for (auto& Image : Asset.images)
            {
                auto& URI = std::get<fastgltf::sources::URI>(Image.data);
                FGLTFImage GLTFImage;
                GLTFImage.ByteOffset = URI.fileByteOffset;
                GLTFImage.RelativePath = URI.uri.c_str();
                OutData.Textures.push_back(GLTFImage);
            }
            
            for (auto& Primitive : Mesh.primitives)
            {
                FGeometrySurface NewSurface;
                NewSurface.StartIndex = IndexCount;
                NewSurface.ID = Mesh.name.empty() ? FName(Name + "_" + eastl::to_string(NewResource.GetNumSurfaces())) : Mesh.name.c_str();
                
                if (Primitive.materialIndex.has_value())
                {
                    NewSurface.MaterialIndex = (int64)Primitive.materialIndex.value();
                }
                
                SIZE_T InitialVert = 0;
        
                fastgltf::Accessor& IndexAccessor = Asset.accessors[Primitive.indicesAccessor.value()];
                NewResource.Indices.reserve(IndexAccessor.count);
        
                fastgltf::iterateAccessor<uint32>(Asset, IndexAccessor, [&](uint32 Index)
                {
                    NewResource.Indices.push_back(Index);
                    NewSurface.IndexCount++;
                    IndexCount++;
                });
        
                fastgltf::Accessor& PosAccessor = Asset.accessors[Primitive.findAttribute("POSITION")->second];
                NewResource.Vertices.resize(NewResource.Vertices.size() + PosAccessor.count);
        
                fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, PosAccessor, [&](glm::vec3 V, size_t Index)
                {
                    FVertex Vertex;
                    Vertex.Position.x = V.x;
                    Vertex.Position.y = V.y;
                    Vertex.Position.z = V.z;
                    Vertex.Position.w = 1.0f;
                    
                    NewResource.Vertices[InitialVert + Index] = Memory::Move(Vertex);
                });
        
                auto normals = Primitive.findAttribute("NORMAL");
                if (normals != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, Asset.accessors[(*normals).second], [&](glm::vec3 v, size_t index)
                    {
                        NewResource.Vertices[InitialVert + index].Normal.x = v.x;
                        NewResource.Vertices[InitialVert + index].Normal.y = v.y;
                        NewResource.Vertices[InitialVert + index].Normal.z = v.z;
                        NewResource.Vertices[InitialVert + index].Normal.w = 1.0f;

                    });
                }
        
                auto uv = Primitive.findAttribute("TEXCOORD_0");
                if (uv != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(Asset, Asset.accessors[(*uv).second], [&](glm::vec2 v, size_t index)
                    {
                        NewResource.Vertices[InitialVert + index].UV.x = v.x;
                        NewResource.Vertices[InitialVert + index].UV.y = v.y;
                    });
                }
        
                // load vertex colors
                auto colors = Primitive.findAttribute("COLOR_0");
                if (colors != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(Asset, Asset.accessors[(*colors).second], [&](glm::vec4 v, size_t index)
                    {
                        NewResource.Vertices[InitialVert + index].Color.r = v.r;
                        NewResource.Vertices[InitialVert + index].Color.g = v.g;
                        NewResource.Vertices[InitialVert + index].Color.b = v.b;
                        NewResource.Vertices[InitialVert + index].Color.a = v.a;
                    });
                }
                NewResource.GeometrySurfaces.push_back(NewSurface);
            }

            OutData.Resources.push_back(NewResource);
        }
    }
    
}
