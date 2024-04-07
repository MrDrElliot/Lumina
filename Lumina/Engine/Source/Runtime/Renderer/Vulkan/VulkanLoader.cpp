#include "VulkanLoader.h"

#include "Source/Runtime/Log/Log.h"
#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include "VulkanRenderContext.h"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/Renderer/RenderContext.h"

std::optional<std::vector<std::shared_ptr<Lumina::FMeshAsset>>> Lumina::LoadGltfMeshes(std::filesystem::path InPath)
{
    LE_LOG_TRACE("Loading GLTF: {0}", InPath.string());

    fastgltf::GltfDataBuffer Data;
    Data.loadFromFile(InPath);

    constexpr auto Options = fastgltf::Options::LoadGLBBuffers;

    fastgltf::Asset gltf;
    fastgltf::Parser parser;

    auto load = parser.loadGltfBinary(&Data, InPath.parent_path(), Options);
    if(load)
    {
        gltf = std::move(load.get());
    }
    else
    {
        LE_LOG_ERROR("Failed to load GLTF {0}", fastgltf::to_underlying(load.error()));
    }
    
     std::vector<std::shared_ptr<FMeshAsset>> meshes;

    // use the same vectors for all meshes so that the memory doesnt reallocate as
    // often
    std::vector<uint32_t> indices;
    std::vector<FVertex> vertices;
    for (fastgltf::Mesh& mesh : gltf.meshes) {
        FMeshAsset newmesh;

        newmesh.Name = mesh.name;

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives) {
            FGeoSurface newSurface;
            newSurface.StartIndex = (uint32_t)indices.size();
            newSurface.Count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = vertices.size();

            // load indexes
            {
                fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexaccessor.count);
                
                fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
                    [&](std::uint32_t idx) {
                        indices.push_back(idx + initial_vtx);
                    });
            }


            // load vertex positions
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                    [&](glm::vec3 v, size_t index) {
                        FVertex newvtx;
                        newvtx.Position = v;
                        newvtx.Normal = { 1, 0, 0 };
                        newvtx.Color = glm::vec4 { 1.f };
                        newvtx.UV_x = 0;
                        newvtx.UV_y = 0;
                        vertices[initial_vtx + index] = newvtx;
                    });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
                    [&](glm::vec3 v, size_t index) {
                        vertices[initial_vtx + index].Normal = v;
                    });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
                    [&](glm::vec2 v, size_t index) {
                        vertices[initial_vtx + index].UV_x = v.x;
                        vertices[initial_vtx + index].UV_y = v.y;
                    });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
                    [&](glm::vec4 v, size_t index) {
                        vertices[initial_vtx + index].Color = v;
                    });
            }
            newmesh.Surfaces.push_back(newSurface);
        }

        // display the vertex normals
        constexpr bool OverrideColors = true;
        if (OverrideColors) {
            for (FVertex& vtx : vertices) {
                vtx.Color = glm::vec4(vtx.Normal, 1.f);
            }
        }

        
        newmesh.MeshBuffers = FRenderContext::Get<FVulkanRenderContext>()->UploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<FMeshAsset>(std::move(newmesh)));
    }

    return meshes;

}
