#include "StaticMeshFactory.h"

#include <fastgltf/core.hpp>

#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/RenderTypes.h"

namespace Lumina
{
    void FStaticMeshFactory::Import(std::filesystem::path InPath)
    {
        fastgltf::Asset Asset;
        
        ExtractAsset(&Asset, InPath);
        
        
        for (auto Mesh : Asset.meshes)
        {
            std::vector<uint32_t> Indices;
            std::vector<FVertex> Vertices;

            for (auto Primitive : Mesh.primitives)
            {
            }
        }
    }

    void FStaticMeshFactory::ExtractAsset(fastgltf::Asset* OutAsset, std::filesystem::path InPath)
    {
        // Allocate crucial fastgltf objects
        fastgltf::Parser gltf_parser;
        fastgltf::GltfDataBuffer data_buffer;

        // Try to load asset data
        if (!data_buffer.loadFromFile(InPath))
        {
            LE_LOG_ERROR("Failed to load glTF model with path: {0}. Aborting import.", InPath.string());
            return;
        }

        fastgltf::GltfType source_type = fastgltf::determineGltfFileType(&data_buffer);

        if (source_type == fastgltf::GltfType::Invalid)
        {
            LE_LOG_ERROR("Failed to determine glTF file type with path: {0}. Aborting import.", InPath.string());
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
            LE_LOG_ERROR("Failed to load asset source with path: {0}. [{1}]: {2} Aborting import.", InPath.string(),
                fastgltf::getErrorName(error), fastgltf::getErrorMessage(error));
        }

        *OutAsset = std::move(expected_asset.get());
    }
}
