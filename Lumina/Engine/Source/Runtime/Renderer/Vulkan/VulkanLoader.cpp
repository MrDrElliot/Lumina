#include "VulkanLoader.h"

#include "Source/Runtime/Log/Log.h"
#include <fastgltf/include/fastgltf/core.hpp>
#include <fastgltf/include/fastgltf/glm_element_traits.hpp>
#include <fastgltf/include/fastgltf/tools.hpp>
#include "VulkanRenderContext.h"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/Assets/StaticMesh/StaticMesh.h"

namespace Lumina
{
    std::optional<std::vector<std::shared_ptr<FStaticMesh>>> Lumina::LoadGltfMeshes(std::filesystem::path FilePath)
    {
        LE_LOG_INFO("Attempting Mesh Load: {0}", FilePath.string());

        fastgltf::GltfDataBuffer DataBuffer;
        DataBuffer.loadFromFile(FilePath);

        constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers
            | fastgltf::Options::LoadExternalBuffers;

        fastgltf::Asset Asset;
        fastgltf::Parser Parser {};

        auto load = Parser.loadGltfBinary(&DataBuffer, FilePath.parent_path(), gltfOptions);
        if (load)
        {
            Asset = std::move(load.get());
        }
        else
        {
            LE_LOG_ERROR("Failed to load asset from file path: {0}", FilePath.string());
            return {};
        }

        std::vector<std::shared_ptr<FStaticMesh>> Meshes;

         // use the same vectors for all meshes so that the memory doesnt reallocate as
    // often
    std::vector<uint32_t> indices;
    std::vector<FVertex> Vertices;
    for (fastgltf::Mesh& mesh : Asset.meshes)
    {
        FStaticMesh NewMesh(FilePath);

        NewMesh.SetName(mesh.name.c_str());

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        Vertices.clear();

        for (auto&& p : mesh.primitives)
        {
            FGeometrySurface NewSurface;
            NewSurface.StartIndex = (uint32_t)indices.size();
            NewSurface.Count = (uint32_t)Asset.accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = Vertices.size();

            fastgltf::Accessor& indexaccessor = Asset.accessors[p.indicesAccessor.value()];
            indices.reserve(indices.size() + indexaccessor.count);

            fastgltf::iterateAccessor<std::uint32_t>(Asset, indexaccessor, [&](std::uint32_t idx)
            {
                indices.push_back(idx + initial_vtx);
            });

            {
                fastgltf::Accessor& posAccessor = Asset.accessors[p.findAttribute("POSITION")->second];
                Vertices.resize(Vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, posAccessor,
                    [&](glm::vec3 v, size_t index)
                    {
                        FVertex newvtx;
                        newvtx.Position = v;
                        newvtx.Normal = { 1, 0, 0 };
                        newvtx.Color = glm::vec4 { 1.f };
                        newvtx.UV_x = 0;
                        newvtx.UV_y = 0;
                        Vertices[initial_vtx + index] = newvtx;
                    });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, Asset.accessors[(*normals).second],
                    [&](glm::vec3 v, size_t index)
                    {
                        Vertices[initial_vtx + index].Normal = v;
                    });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec2>(Asset, Asset.accessors[(*uv).second], [&](glm::vec2 v, size_t index)
                {
                    Vertices[initial_vtx + index].UV_x = v.x;
                    Vertices[initial_vtx + index].UV_y = v.y;
                });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<glm::vec4>(Asset, Asset.accessors[(*colors).second], [&](glm::vec4 v, size_t index)
                {
                    Vertices[initial_vtx + index].Color = v;
                });
            }
            NewMesh.GetSurfaces().push_back(NewSurface);
        }

        // display the vertex normals
        constexpr bool OverrideColors = true;
        if (OverrideColors)
        {
            for (FVertex& vtx : Vertices)
            {
                vtx.Color = glm::vec4(vtx.Normal, 1.f);
            }
        }
        
        
        NewMesh.GetMeshBuffers() = FRenderContext::Get<FVulkanRenderContext>()->UploadMesh(indices, Vertices);

        Meshes.emplace_back(std::make_shared<FStaticMesh>(std::move(NewMesh)));
    }

    return Meshes;
        
    }
}
