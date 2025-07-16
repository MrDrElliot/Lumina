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
#include "Renderer/RenderContext.h"
#include "Renderer/RenderResource.h"
#include "Renderer/Vertex.h"
#include "stb_image/stb_image.h"

namespace Lumina::ImportHelpers
{
    namespace
    {
        void ExtractAsset(fastgltf::Asset* OutAsset, const FString& InPath)
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
    }
    
    
    FIntVector2D GetImagePixelData(TVector<uint8>& OutPixels, const FString& RawFilePath, bool bFlipVertical)
    {
        OutPixels.clear();
        
        stbi_set_flip_vertically_on_load(bFlipVertical);

        int x, y, c;
        stbi_uc* data = stbi_load(RawFilePath.c_str(), &x, &y, &c, STBI_rgb_alpha);
        if (data == nullptr)
        {
            LOG_WARN("Failed to get image pixel data from: {0}", RawFilePath);
            return {};
        }
        
        OutPixels.assign(data, data + static_cast<size_t>(x) * static_cast<size_t>(y) * STBI_rgb_alpha);
        stbi_image_free(data);
        
        return {x, y};
    }

    void GetMeshVertexData(TVector<FVertex>& OutVertices, TVector<uint32>& OutIndices, const FString& RawFilePath)
    {
        fastgltf::Asset Asset;
        ExtractAsset(&Asset, RawFilePath);

        for (fastgltf::Mesh& Mesh : Asset.meshes)
        {
            for (auto& Primitive : Mesh.primitives)
            {
                size_t InitialVert = 0;
        
                fastgltf::Accessor& IndexAccessor = Asset.accessors[Primitive.indicesAccessor.value()];
                OutIndices.reserve(IndexAccessor.count);
        
                fastgltf::iterateAccessor<std::uint32_t>(Asset, IndexAccessor, [&](std::uint32_t Index)
                {
                    OutIndices.push_back(Index);
                });
        
                fastgltf::Accessor& PosAccessor = Asset.accessors[Primitive.findAttribute("POSITION")->second];
                OutVertices.resize(OutVertices.size() + PosAccessor.count);
        
                fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, PosAccessor, [&](glm::vec3 V, size_t Index)
                {
                    FVertex Vertex;
                    Vertex.Position.x = V.x;
                    Vertex.Position.y = V.y;
                    Vertex.Position.z = V.z;
                    Vertex.Position.w = 1.0f;
                    
                    OutVertices[InitialVert + Index] = Memory::Move(Vertex);
                });
        
                auto normals = Primitive.findAttribute("NORMAL");
                if (normals != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(Asset, Asset.accessors[(*normals).second], [&](glm::vec3 v, size_t index)
                    {
                        OutVertices[InitialVert + index].Normal.x = v.x;
                        OutVertices[InitialVert + index].Normal.y = v.y;
                        OutVertices[InitialVert + index].Normal.z = v.z;
                        OutVertices[InitialVert + index].Normal.w = 1.0f;

                    });
                }
        
                auto uv = Primitive.findAttribute("TEXCOORD_0");
                if (uv != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(Asset, Asset.accessors[(*uv).second], [&](glm::vec2 v, size_t index)
                    {
                        OutVertices[InitialVert + index].UV.x = v.x;
                        OutVertices[InitialVert + index].UV.y = v.y;
                    });
                }
        
                // load vertex colors
                auto colors = Primitive.findAttribute("COLOR_0");
                if (colors != Primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(Asset, Asset.accessors[(*colors).second], [&](glm::vec4 v, size_t index)
                    {
                        OutVertices[InitialVert + index].Color.r = v.r;
                        OutVertices[InitialVert + index].Color.g = v.g;
                        OutVertices[InitialVert + index].Color.b = v.b;
                        OutVertices[InitialVert + index].Color.a = v.a;
                    });
                }
            }
        }
    }

    
    FRHIImageRef CreateTextureFromImport(IRenderContext* RenderContext, const FString& RawFilePath, bool bFlipVerticalOnLoad)
    {
        TVector<uint8> Pixels;
        FRHIImageDesc ImageDescription;
        ImageDescription.Format = EFormat::RGBA8_UNORM;
        ImageDescription.Extent = GetImagePixelData(Pixels, RawFilePath, bFlipVerticalOnLoad);
        ImageDescription.Flags.SetFlag(EImageCreateFlags::ShaderResource);
        ImageDescription.NumMips = 1;
        ImageDescription.DebugName = Paths::FileName(RawFilePath, true);
        ImageDescription.InitialState = EResourceStates::ShaderResource;
        ImageDescription.bKeepInitialState = true;
        
        FRHIImageRef ReturnImage = RenderContext->CreateImage(ImageDescription);

        const uint32 Width = ImageDescription.Extent.X;
        const uint32 Height = ImageDescription.Extent.Y;
        const SIZE_T RowPitch = Width * 4;
        const SIZE_T DepthPitch = RowPitch * Height;

        FRHICommandListRef TransferCommandList = RenderContext->CreateCommandList(FCommandListInfo::Graphics());
        TransferCommandList->Open();
        TransferCommandList->WriteImage(ReturnImage, 0, 0, Pixels.data(), RowPitch, DepthPitch);
        TransferCommandList->Close();
        RenderContext->ExecuteCommandList(TransferCommandList, 1, Q_Graphics);

        return ReturnImage;
    }
}
