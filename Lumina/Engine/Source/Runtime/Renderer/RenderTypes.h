#pragma once

#include "Lumina.h"
#include <glm/glm.hpp>
#include "Core/Math/Color.h"
#include "Vertex.h"
#include "Core/Math/Hash/Hash.h"
#include "Core/Serialization/Archiver.h"
#include "Core/Templates/CanBulkSerialize.h"

#define NO_TEXTURE (-1)
#define FRAMES_IN_FLIGHT 2

namespace Lumina
{
    struct FVertex;
    struct FVertexAttribute;
    class FShader;

    struct FMaterialTexturesData final
    {
        int32 AlbedoID =        NO_TEXTURE;
        int32 NormalID =        NO_TEXTURE;
        int32 RoughnessID =     NO_TEXTURE;
        int32 EmissiveID =      NO_TEXTURE;
        int32 AOID =            NO_TEXTURE;
        int32 Padding[3] =      {0, 0, 0};
    };
    
    struct FMeshModelData
    {
        glm::mat4               ModelMatrix;
        FMaterialTexturesData   MaterialTextureData;
    };
    
    enum class EDescriptorBindingType : uint32
    {
        SAMPLER                       = 0,
        COMBINED_IMAGE_SAMPLER        = 1,
        SAMPLED_IMAGE                 = 2,
        STORAGE_IMAGE                 = 3,
        UNIFORM_TEXEL_BUFFER          = 4,
        STORAGE_TEXEL_BUFFER          = 5,
        UNIFORM_BUFFER                = 6,
        STORAGE_BUFFER                = 7,
        UNIFORM_BUFFER_DYNAMIC        = 8,
        STORAGE_BUFFER_DYNAMIC        = 9,
        INPUT_ATTACHMENT              = 10,
        ACCELERATION_STRUCTURE_KHR    = 1000150000
    };

    
    struct FDescriptorBinding
    {
        FName Name;
        uint32 Size;
        uint32 Count;
        uint16 Set;
        uint16 Binding;               // The binding index
        EDescriptorBindingType Type;  // Type of descriptor (e.g., uniform buffer, texture)
        uint32 ArrayCount;            // Number of descriptors in the array
        uint64 Flags;                 // Additional flags for binding (e.g., PARTIALLY_BOUND)
        uint32 StageFlags;            // Shader stage this binding applies to (VERTEX, FRAGMENT, etc.)
    };

    enum class EImageLayout
    {
        UNDEFINED = 0,
        GENERAL = 1,
        COLOR_ATTACHMENT = 2,
        DEPTH_STENCIL_ATTACHMENT = 3,
        DEPTH_STENCIL_READ_ONLY = 4,
        SHADER_READ_ONLY = 5,
        TRANSFER_SRC = 6,
        TRANSFER_DST = 7,
        PRESENT_SRC = 1000001002
    };

    enum class EImageUsage : glm::uint8
    {
        TEXTURE,
        RENDER_TARGET,
        DEPTH_BUFFER,
        RESOLVE,
    };

    enum class EImageFormat : glm::uint8
    {
        R8,
        RB16,
        RGB24,
        RGBA32_SRGB,
        RGBA32_UNORM,
        BGRA32_SRGB,
        BGRA32_UNORM,
        RGB32_HDR,
        RGBA64_HDR,
        RGBA128_HDR,
        D32,
        BC1,
        BC5,
        BC6h,
        BC7,
        RGBA64_SFLOAT,
        RGB24_UNORM
    };
    
    enum class EImageType : glm::uint8
    {
        TYPE_1D,
        TYPE_2D,
        TYPE_3D,
    };

    enum class EImageSampleCount : uint8
    {
        ONE,
        FOUR,
    };

    enum class EPipelineType : uint8
    {
        GRAPHICS,
        COMPUTE,
        RAY_TRACING
    };

    enum class EPipelineCullingMode : uint8
    {
        BACK,
        FRONT,
        NONE
    };

    enum class EPipelineFrontFace : uint8
    {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    enum class EPipelineTopology : uint8
    {
        TRIANGLES,
        LINES,
        POINTS
    };

    enum class EPipelineBlending : uint8
    {
        BLEND_FACTOR_ZERO = 0,
        BLEND_FACTOR_ONE = 1,
    };

    enum class EPipelineFillMode : uint8
    {
        FILL,
        EDGE_ONLY
    };

    enum class EShaderDataType : uint8
    {
        INT,
        INT2,
        INT3,
        INT4,

        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,

        IMAT3,
        IMAT4,

        MAT3,
        MAT4
    };

    constexpr uint32 DeviceDataTypeSize(const EShaderDataType& Type)
    {
        switch (Type)
        {
            case EShaderDataType::INT:           return sizeof(int);
            case EShaderDataType::INT2:          return sizeof(int) * 2;
            case EShaderDataType::INT3:          return sizeof(int) * 3;
            case EShaderDataType::INT4:          return sizeof(int) * 4;
            case EShaderDataType::FLOAT:         return sizeof(float);
            case EShaderDataType::FLOAT2:        return sizeof(float) * 2;
            case EShaderDataType::FLOAT3:        return sizeof(float) * 3;
            case EShaderDataType::FLOAT4:        return sizeof(float) * 4;
            case EShaderDataType::IMAT3:         return sizeof(int) * 3 * 3;
            case EShaderDataType::IMAT4:         return sizeof(int) * 4 * 4;
            case EShaderDataType::MAT3:          return sizeof(float) * 3 * 3;
            case EShaderDataType::MAT4:          return sizeof(float) * 4 * 4;
            default:                             std::unreachable();
        }
    }

    struct DeviceBufferLayoutElement
    {
        DeviceBufferLayoutElement(EShaderDataType InFormat)
            : Format(InFormat)
        {}
        
        bool operator==(const DeviceBufferLayoutElement& other) const
        {
            bool result = true;
            result &= Format == other.Format;
            result &= Size == other.Size;
            result &= Offset == other.Offset;

            return result;
        }

        EShaderDataType Format = EShaderDataType::FLOAT4;
        uint32 Size = 0;
        uint32 Offset = 0;
    };

    class FDeviceBufferLayout
    {
    public:
        FDeviceBufferLayout() {}
        FDeviceBufferLayout(TVector<DeviceBufferLayoutElement> list)
            : Elements(eastl::move(list))
        {
            for (auto& element : Elements)
            {
                uint32 datasize = DeviceDataTypeSize(element.Format);
                element.Offset = Stride;
                element.Size = datasize;
                Stride += datasize;
            }
        }
        
        uint32 GetStride() const { return Stride; }
        const TVector<DeviceBufferLayoutElement>& GetElements() const { return Elements; }
        const TVector<DeviceBufferLayoutElement>::iterator begin() { return Elements.begin(); }
        const TVector<DeviceBufferLayoutElement>::iterator end() { return Elements.end(); }

    private:

        TVector<DeviceBufferLayoutElement> Elements;
        uint32 Stride = 0;
    };

    template <typename T>
    struct FVertexTypeTraits;
    

    struct FPipelineSpec
    {

        static FPipelineSpec Create() { return FPipelineSpec(); }
        
        struct FVertexBinding
        {
            uint32 Stride = 0;
            uint32 Binding = 0;
            uint32 InputRate = 0;
        };

        struct FVertexAttribute
        {
            uint32          Location = 0;
            uint32          Binding = 0;
            EShaderDataType Format = EShaderDataType::INT;
            uint32          Offset = 0;
        };
        
        // General pipeline properties
        FName                       Shader =                    FName();
        float                       LineWidth =                 1.0f;
        EPipelineType               PipelineType =              EPipelineType::GRAPHICS;
        EPipelineCullingMode        CullingMode =               EPipelineCullingMode::BACK;
        EPipelineBlending           AlphaBlendSrcFactor =       EPipelineBlending::BLEND_FACTOR_ONE;
        EPipelineBlending           AlphaBlendDstFactor =       EPipelineBlending::BLEND_FACTOR_ZERO;
        EPipelineFrontFace          FaceOrientation =           EPipelineFrontFace::COUNTER_CLOCKWISE;
        EPipelineTopology           PrimitiveTopology =         EPipelineTopology::TRIANGLES;
        EPipelineFillMode           PolygonFillMode =           EPipelineFillMode::FILL;
        TVector<EImageFormat>       RenderTargetFormats         { EImageFormat::RGBA32_SRGB };

        FVertexBinding              VertexBinding;
        TVector<FVertexAttribute>   VertexAttributes;
        
        uint8                       EnablePrimitiveRestart : 1 =          false;
        uint8                       EnableAlphaBlending : 1 =             true;
        uint8                       EnableDepthTest : 1 =                 true;
        uint8                       EnableMultisampling : 1 =             false;
        uint8                       UseConservativeRasterization : 1 =    false;
        uint8                       SampleCount : 1 =                     1;
        
        bool                        bEmissive =                            false;
        bool                        bTransparent =                         false;
        
        bool operator==(const FPipelineSpec& other) const
        {
            bool result = true;
            result &= Shader == other.Shader;
            result &= LineWidth == other.LineWidth;
            result &= PipelineType == other.PipelineType;
            result &= CullingMode == other.CullingMode;
            result &= FaceOrientation == other.FaceOrientation;
            result &= PrimitiveTopology == other.PrimitiveTopology;
            result &= PolygonFillMode == other.PolygonFillMode;
            result &= EnableAlphaBlending == other.EnableAlphaBlending;
            result &= EnableDepthTest == other.EnableDepthTest;
            result &= SampleCount == other.SampleCount;
            result &= bEmissive == other.bEmissive;
            result &= bTransparent == other.bTransparent;
    
            return result;
        }

        size_t GetHash() const
        {
            size_t hash = 0;
            Hash::HashCombine(hash, eastl::hash<FName>{}(Shader));
            Hash::HashCombine(hash, std::hash<float>{}(LineWidth));
            Hash::HashCombine(hash, std::hash<int>{}(static_cast<int>(PipelineType)));
            Hash::HashCombine(hash, std::hash<int>{}(static_cast<int>(CullingMode)));
            Hash::HashCombine(hash, std::hash<int>{}(static_cast<int>(FaceOrientation)));
            Hash::HashCombine(hash, std::hash<int>{}(static_cast<int>(PrimitiveTopology)));
            Hash::HashCombine(hash, std::hash<int>{}(static_cast<int>(PolygonFillMode)));
            Hash::HashCombine(hash, std::hash<bool>{}(EnableAlphaBlending));
            Hash::HashCombine(hash, std::hash<bool>{}(EnableDepthTest));
            Hash::HashCombine(hash, std::hash<uint8_t>{}(SampleCount));
            Hash::HashCombine(hash, std::hash<bool>{}(bEmissive));
            Hash::HashCombine(hash, std::hash<bool>{}(bTransparent));
            
            return hash;
        }

        template <typename T>
        FPipelineSpec& SetVertexBinding()
        {
            VertexBinding.Binding = 0;
            VertexBinding.Stride = FVertexTypeTraits<T>::Stride;
            FVertexTypeTraits<T>::GetVertexAttributes(VertexAttributes);

            return *this;
        }
        
        FPipelineSpec& SetShader(FName InShader)
        {
            Shader = InShader;
            return *this;
        }
    
        FPipelineSpec& SetLineWidth(float width)
        {
            LineWidth = width;
            return *this;
        }
    
        FPipelineSpec& SetPipelineType(EPipelineType pipelineType)
        {
            PipelineType = pipelineType;
            return *this;
        }
    
        FPipelineSpec& SetCullingMode(EPipelineCullingMode mode)
        {
            CullingMode = mode;
            return *this;
        }
    
        FPipelineSpec& SetAlphaBlendSrcFactor(EPipelineBlending factor)
        {
            AlphaBlendSrcFactor = factor;
            return *this;
        }
    
        FPipelineSpec& SetAlphaBlendDstFactor(EPipelineBlending factor)
        {
            AlphaBlendDstFactor = factor;
            return *this;
        }
    
        FPipelineSpec& SetFaceOrientation(EPipelineFrontFace face)
        {
            FaceOrientation = face;
            return *this;
        }
    
        FPipelineSpec& SetPrimitiveTopology(EPipelineTopology topology)
        {
            PrimitiveTopology = topology;
            return *this;
        }
    
        FPipelineSpec& SetPolygonFillMode(EPipelineFillMode mode)
        {
            PolygonFillMode = mode;
            return *this;
        }
    
        FPipelineSpec& SetRenderTargetFormats(const TVector<EImageFormat>& formats)
        {
            RenderTargetFormats = formats;
            return *this;
        }
    
        FPipelineSpec& SetEnablePrimitiveRestart(bool enable)
        {
            EnablePrimitiveRestart = enable;
            return *this;
        }
    
        FPipelineSpec& SetEnableAlphaBlending(bool enable)
        {
            EnableAlphaBlending = enable;
            return *this;
        }
    
        FPipelineSpec& SetEnableDepthTest(bool enable)
        {
            EnableDepthTest = enable;
            return *this;
        }
    
        FPipelineSpec& SetEnableMultisampling(bool enable)
        {
            EnableMultisampling = enable;
            return *this;
        }
    
        FPipelineSpec& SetUseConservativeRasterization(bool enable)
        {
            UseConservativeRasterization = enable;
            return *this;
        }
    
        FPipelineSpec& SetSampleCount(uint8 count)
        {
            SampleCount = count;
            return *this;
        }
        
        FPipelineSpec& SetIsEmissive(bool emissive)
        {
            bEmissive = emissive;
            return *this;
        }
    
        FPipelineSpec& SetIsTransparent(bool transparent)
        {
            bTransparent = transparent;
            return *this;
        }
    
        // Getters
        FName GetShader() const { return Shader; }
        float GetLineWidth() const { return LineWidth; }
        EPipelineType GetPipelineType() const { return PipelineType; }
        EPipelineCullingMode GetCullingMode() const { return CullingMode; }
        EPipelineBlending GetAlphaBlendSrcFactor() const { return AlphaBlendSrcFactor; }
        EPipelineBlending GetAlphaBlendDstFactor() const { return AlphaBlendDstFactor; }
        EPipelineFrontFace GetFaceOrientation() const { return FaceOrientation; }
        EPipelineTopology GetPrimitiveTopology() const { return PrimitiveTopology; }
        EPipelineFillMode GetPolygonFillMode() const { return PolygonFillMode; }
        const TVector<EImageFormat>& GetRenderTargetFormats() const { return RenderTargetFormats; }
        bool GetEnablePrimitiveRestart() const { return EnablePrimitiveRestart; }
        bool GetEnableAlphaBlending() const { return EnableAlphaBlending; }
        bool GetEnableDepthTest() const { return EnableDepthTest; }
        bool GetEnableMultisampling() const { return EnableMultisampling; }
        bool GetUseConservativeRasterization() const { return UseConservativeRasterization; }
        uint8 GetSampleCount() const { return SampleCount; }
        
        // Material-specific getters
        bool IsEmissive() const { return bEmissive; }
        bool IsTransparent() const { return bTransparent; }
    };

    template <>
    struct FVertexTypeTraits<FVertex>
    {
        static constexpr uint32 Stride = sizeof(FVertex);
        static constexpr uint32 Binding = 0;

        static void GetVertexAttributes(TVector<FPipelineSpec::FVertexAttribute>& OutAttributes)
        {
            FPipelineSpec::FVertexAttribute Attribute;
            Attribute.Location = 0;
            Attribute.Binding = Binding;
            Attribute.Format = EShaderDataType::FLOAT3;
            Attribute.Offset = offsetof(FVertex, Position);
            
            OutAttributes.push_back(Attribute);

            Attribute.Location = 1;
            Attribute.Binding = Binding;
            Attribute.Format = EShaderDataType::FLOAT4;
            Attribute.Offset = offsetof(FVertex, Color);
            
            OutAttributes.push_back(Attribute);

            Attribute.Location = 2;
            Attribute.Binding = Binding;
            Attribute.Format = EShaderDataType::FLOAT3;
            Attribute.Offset = offsetof(FVertex, Normal);
            
            OutAttributes.push_back(Attribute);

            Attribute.Location = 3;
            Attribute.Binding = Binding;
            Attribute.Format = EShaderDataType::FLOAT2;
            Attribute.Offset = offsetof(FVertex, UV);
            
            OutAttributes.push_back(Attribute);
            
        }
    };

    template <>
    struct FVertexTypeTraits<FSimpleElementVertex>
    {
        static constexpr uint32 Stride = sizeof(FSimpleElementVertex);
        static constexpr uint32 Binding = 0;

        static void GetVertexAttributes(TVector<FPipelineSpec::FVertexAttribute>& OutAttributes)
        {
            FPipelineSpec::FVertexAttribute Attribute;
            Attribute.Location = 0;
            Attribute.Binding = Binding;
            Attribute.Format = EShaderDataType::FLOAT3;
            Attribute.Offset = offsetof(FSimpleElementVertex, Position);
            
            OutAttributes.push_back(Attribute);

            Attribute.Location = 1;
            Attribute.Binding = Binding;
            Attribute.Format = EShaderDataType::FLOAT3;
            Attribute.Offset = offsetof(FSimpleElementVertex, Color);
            
            
            OutAttributes.push_back(Attribute);
            
        }
    };
}
