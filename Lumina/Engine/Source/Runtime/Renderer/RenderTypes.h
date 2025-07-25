﻿#pragma once

#include "Lumina.h"
#include <glm/glm.hpp>

#include "Format.h"
#include "RHIFwd.h"
#include "Core/Math/Color.h"
#include "Vertex.h"
#include "Core/LuminaMacros.h"
#include "Core/Math/Hash/Hash.h"

#define NO_TEXTURE (-1)
#define FRAMES_IN_FLIGHT 2
#define SWAPCHAIN_IMAGES 3

namespace Lumina
{
    struct FVertex;
    struct FVertexAttribute;

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

    enum class ERHIAccess : uint32
    {
        None = 0,                            // No access
        
        // General Access Types
        Read = 1 << 0,                       // General read access
        Write = 1 << 1,                      // General write access
        
        // Transfer Access Types
        TransferRead = 1 << 2,               // Read access in a transfer operation (e.g., copying)
        TransferWrite = 1 << 3,              // Write access in a transfer operation (e.g., copying)
        
        // Shader Access Types
        ShaderRead = 1 << 4,                 // Read access in a shader (e.g., from a texture or buffer)
        ShaderWrite = 1 << 5,                // Write access in a shader (e.g., to a texture or buffer)
        
        // Color Attachment Access Types
        ColorAttachmentWrite = 1 << 6,       // Write access to a color attachment (e.g., during rendering)
        
        // Depth/Stencil Attachment Access Types
        DepthStencilAttachmentWrite = 1 << 7, // Write access to a depth/stencil attachment (e.g., during rendering)
        
        // Present Access Types
        PresentRead = 1 << 8,                // Read access for presenting an image (e.g., to the screen)
        
        // Miscellaneous Access Types
        HostRead = 1 << 9,                   // Read access for CPU (host-side access to resources)
        HostWrite = 1 << 10,                  // Write access for CPU (host-side access to resources)
        
        // Compute Access Types
        ComputeRead = 1 << 11,               // Read access in compute shaders
        ComputeWrite = 1 << 12,              // Write access in compute shaders

        General = HostRead,
        
        // Combined Access Flags
        All = Read | Write | TransferRead | TransferWrite | ShaderRead | ShaderWrite | ColorAttachmentWrite |
          DepthStencilAttachmentWrite | PresentRead | HostRead | HostWrite | ComputeRead | ComputeWrite
    };

    enum class EResourceStates : uint32
    {
        Unknown                     = 0,
        Common                      = 0x00000001,
        ConstantBuffer              = 0x00000002,
        VertexBuffer                = 0x00000004,
        IndexBuffer                 = 0x00000008,
        IndirectArgument            = 0x00000010,
        ShaderResource              = 0x00000020,
        UnorderedAccess             = 0x00000040,
        RenderTarget                = 0x00000080,
        DepthWrite                  = 0x00000100,
        DepthRead                   = 0x00000200,
        StreamOut                   = 0x00000400,
        CopyDest                    = 0x00000800,
        CopySource                  = 0x00001000,
        ResolveDest                 = 0x00002000,
        ResolveSource               = 0x00004000,
        Present                     = 0x00008000,
        AccelStructRead             = 0x00010000,
        AccelStructWrite            = 0x00020000,
        AccelStructBuildInput       = 0x00040000,
        AccelStructBuildBlas        = 0x00080000,
        ShadingRateSurface          = 0x00100000,
        OpacityMicromapWrite        = 0x00200000,
        OpacityMicromapBuildInput   = 0x00400000,
    };

    ENUM_CLASS_FLAGS(EResourceStates)

    enum class ERHIPipeline : uint8
    {
        Graphics = 1 << 0,
        AsyncCompute = 1 << 1,

        None = 0,
        All = Graphics | AsyncCompute,
        Num = 2
    };

    enum class ECommandQueue : uint8
    {
        Graphics    = 0,
        Compute     = 1,
        Transfer    = 2,
        
        Num         = 3,
    };

    #define Q_Graphics ECommandQueue::Graphics
    #define Q_Compute ECommandQueue::Compute
    #define Q_Transfer ECommandQueue::Transfer

    
    enum class ECommandBufferUsage : uint8
    {
        General,
        Transient,
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
    };

    enum class EBufferUsageFlags : uint32
    {
        None = 0,
        NullResource,
        Dynamic,
        SourceCopy,
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        IndirectBuffer,
        StorageBuffer,
        StagingBuffer,
        CPUWritable,
        CPUReadable,
    };
    
    ENUM_CLASS_FLAGS(EBufferUsageFlags);

    #define BUF_NullResource    EBufferUsageFlags::NullResource
    #define BUF_Dynamic         EBufferUsageFlags::Dynamic
    #define BUF_SourceCopy      EBufferUsageFlags::SourceCopy
    #define BUF_VertexBuffer    EBufferUsageFlags::VertexBuffer
    #define BUF_IndexBuffer     EBufferUsageFlags::IndexBuffer
    #define BUF_UniformBuffer   EBufferUsageFlags::UniformBuffer
    #define BUF_StorageBuffer   EBufferUsageFlags::StorageBuffer
    #define BUF_Indirect        EBufferUsageFlags::IndirectBuffer
    
    enum class ERenderDeviceBufferMemoryUsage : uint8
    {
        None              = 0,
        GPUOnly           = 1 << 0,  // (Fast GPU memory, not CPU accessible)
        CPUToGPU          = 1 << 1,  // (Staging, mapped)
        GPUToCPU          = 1 << 2,  // (Readable from GPU)
        Transient         = 1 << 3,  // (For transient resources)
    };

    enum class ERenderLoadOp : uint8
    {
        Clear,      // Clear attachment to a specified color/depth
        Load,       // Load existing contents
    };

    enum class ERenderStoreOp : uint8
    {
        Store,
        DontCare,
    };

    
    
    struct FRenderPassBeginInfo
    {
        bool bValidPass = false;
        TVector<FRHIImageRef> ColorAttachments;    // Color attachments
        TVector<ERenderLoadOp> ColorLoadOps;       // One per color attachment
        TVector<ERenderStoreOp> ColorStoreOps;     // How to handle color attachments at the end
        TVector<FColor> ClearColorValues;          // Optional clear values for color attachments
        
        FRHIImageRef DepthAttachment;                           // Single depth attachment
        ERenderLoadOp DepthLoadOp = ERenderLoadOp::Clear;       // Default depth load operation
        ERenderStoreOp DepthStoreOp = ERenderStoreOp::Store;    // Default depth store operation
        float ClearDepth = 1.0f;                                // Default depth clear value
        uint32 ClearStencil = 0;                                // Default stencil clear value
        FString DebugName;
        FIntVector2D RenderArea;                                // Defines the renderable area
    
        // Fluent API for adding color attachments
        FRenderPassBeginInfo& AddColorAttachment(const FRHIImageRef& Attachment)
        {
            ColorAttachments.push_back(Attachment);
            return *this;
        }
    
        // Set a LoadOp for the most recently added color attachment
        FRenderPassBeginInfo& SetColorLoadOp(ERenderLoadOp LoadOp)
        {
            ColorLoadOps.push_back(LoadOp);
            return *this;
        }
    
        // Set a StoreOp for the most recently added color attachment
        FRenderPassBeginInfo& SetColorStoreOp(ERenderStoreOp StoreOp)
        {
            ColorStoreOps.push_back(StoreOp);
            return *this;
        }
    
        // Set the clear color for the most recently added color attachment
        FRenderPassBeginInfo& SetColorClearColor(const FColor& ClearColor)
        {
            ClearColorValues.push_back(ClearColor);
            return *this;
        }
    
        // Fluent API for adding a depth attachment
        FRenderPassBeginInfo& SetDepthAttachment(const FRHIImageRef& Attachment)
        {
            DepthAttachment = Attachment;
            return *this;
        }
    
        // Set a LoadOp for the depth attachment
        FRenderPassBeginInfo& SetDepthLoadOp(ERenderLoadOp LoadOp)
        {
            DepthLoadOp = LoadOp;
            return *this;
        }
    
        // Set a StoreOp for the depth attachment
        FRenderPassBeginInfo& SetDepthStoreOp(ERenderStoreOp StoreOp)
        {
            DepthStoreOp = StoreOp;
            return *this;
        }
    
        // Set clear depth value for the depth attachment
        FRenderPassBeginInfo& SetDepthClearValue(float Depth)
        {
            ClearDepth = Depth;
            return *this;
        }
    
        // Set clear stencil value for the depth attachment
        FRenderPassBeginInfo& SetDepthClearStencil(uint32 Stencil)
        {
            ClearStencil = Stencil;
            return *this;
        }
    
        // Set the render area
        FRenderPassBeginInfo& SetRenderArea(const FIntVector2D& Area)
        {
            RenderArea = Area;
            return *this;
        }
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

    

    /** Describes the dimension of a texture. */
    enum class EImageDimension : uint8
    {
        Unknown,
        Texture2D,
        Texture2DArray,
        Texture3D,
        TextureCube,
        TextureCubeArray
    };

    enum class EImageCreateFlags : uint32
    {
        /** No special flags */
        None = 0,

        /** Texture can be presented to the screen (e.g., used as a swapchain image) */
        Presentable = 1,

        /** Texture can be used as a render target (color or depth attachment) */
        RenderTarget = 2,

        /** Texture can be used as a shader resource (sampled in shaders) */
        ShaderResource = 3,

        /** Texture can be used as a storage image (read/write access in compute shaders) */
        Storage = 4,

        /** Texture supports input attachments (used in Vulkan subpasses) */
        InputAttachment = 5,

        /** Texture can be used as a depth/stencil buffer */
        DepthStencil = 6,

        /** Texture allows unordered access (DirectX-style UAV equivalent) */
        UnorderedAccess = 7,

        /** Texture supports mipmap generation */
        GenerateMipMaps = 8,

        /** Texture can be used as a cube map */
        CubeCompatible = 9,

        /** Texture supports aliasing (can be used with sparse memory allocation) */
        Aliasable = 10,

        /** Texture can be used with multi-sampling (MSAA) */
        MultiSampled = 11,


        // Aliases.

        ColorAttachment = RenderTarget,
        DepthAttachment = DepthStencil,
    };
    
    enum class EImageType : uint8
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
        Graphics,
        Compute,
        RayTracing
    };

    enum class EPipelineStage : uint8
    {
        // Common pipeline stages
        TopOfPipe,                  // No operation, beginning of the pipeline
        BottomOfPipe,               // End of the pipeline

        // Vertex Input stages
        VertexInput,                // Vertex data is being read from buffers
        VertexShader,               // Vertex shader execution

        // Tessellation stages
        TessellationControlShader,  // Tessellation control shader execution
        TessellationEvaluationShader, // Tessellation evaluation shader execution

        // Geometry stages
        GeometryShader,             // Geometry shader execution

        // Fragment stages
        FragmentShader,             // Fragment shader execution
        ColorAttachmentOutput,      // After the fragment shader and before blending

        // Compute stages
        ComputeShader,              // Compute shader execution
        Transfer,                   // Data transfer (e.g., buffers or images)

        // Post-processing stages
        EarlyFragmentTests,         // Early fragment testing (e.g., depth/stencil testing)
        LateFragmentTests,          // Late fragment testing (e.g., depth/stencil testing after blending)

        // Special stages
        Host,                       // Operations that occur on the host (CPU)
        AllGraphics,                // All stages related to graphics pipeline
        AllCommands                 // All stages (graphics, compute, etc.)
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
    
    template <typename T>
    struct FVertexTypeTraits;
    

    struct FGraphicsPipelineSpec
    {

        static FGraphicsPipelineSpec Create() { return FGraphicsPipelineSpec(); }
        
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
        EPipelineType               PipelineType =              EPipelineType::Graphics;
        EPipelineCullingMode        CullingMode =               EPipelineCullingMode::BACK;
        EPipelineBlending           AlphaBlendSrcFactor =       EPipelineBlending::BLEND_FACTOR_ONE;
        EPipelineBlending           AlphaBlendDstFactor =       EPipelineBlending::BLEND_FACTOR_ZERO;
        EPipelineFrontFace          FaceOrientation =           EPipelineFrontFace::COUNTER_CLOCKWISE;
        EPipelineTopology           PrimitiveTopology =         EPipelineTopology::TRIANGLES;
        EPipelineFillMode           PolygonFillMode =           EPipelineFillMode::FILL;
        TVector<EFormat>            RenderTargetFormats         { EFormat::SRGBA8_UNORM };

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
        
        bool operator==(const FGraphicsPipelineSpec& other) const
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
        FGraphicsPipelineSpec& SetVertexBinding()
        {
            VertexBinding.Binding = 0;
            VertexBinding.Stride = FVertexTypeTraits<T>::Stride;
            FVertexTypeTraits<T>::GetVertexAttributes(VertexAttributes);

            return *this;
        }
        
        FGraphicsPipelineSpec& SetShader(FName InShader)
        {
            Shader = InShader;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetLineWidth(float width)
        {
            LineWidth = width;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetPipelineType(EPipelineType pipelineType)
        {
            PipelineType = pipelineType;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetCullingMode(EPipelineCullingMode mode)
        {
            CullingMode = mode;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetAlphaBlendSrcFactor(EPipelineBlending factor)
        {
            AlphaBlendSrcFactor = factor;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetAlphaBlendDstFactor(EPipelineBlending factor)
        {
            AlphaBlendDstFactor = factor;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetFaceOrientation(EPipelineFrontFace face)
        {
            FaceOrientation = face;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetPrimitiveTopology(EPipelineTopology topology)
        {
            PrimitiveTopology = topology;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetPolygonFillMode(EPipelineFillMode mode)
        {
            PolygonFillMode = mode;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetRenderTargetFormats(const TVector<EFormat>& formats)
        {
            RenderTargetFormats = formats;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetEnablePrimitiveRestart(bool enable)
        {
            EnablePrimitiveRestart = enable;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetEnableAlphaBlending(bool enable)
        {
            EnableAlphaBlending = enable;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetEnableDepthTest(bool enable)
        {
            EnableDepthTest = enable;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetEnableMultisampling(bool enable)
        {
            EnableMultisampling = enable;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetUseConservativeRasterization(bool enable)
        {
            UseConservativeRasterization = enable;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetSampleCount(uint8 count)
        {
            SampleCount = count;
            return *this;
        }
        
        FGraphicsPipelineSpec& SetIsEmissive(bool emissive)
        {
            bEmissive = emissive;
            return *this;
        }
    
        FGraphicsPipelineSpec& SetIsTransparent(bool transparent)
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
        const TVector<EFormat>& GetRenderTargetFormats() const { return RenderTargetFormats; }
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

    struct FVertexBufferBinding
    {
        FRHIBuffer* Buffer = nullptr;
        uint32 Slot;
        uint64 Offset;

        bool operator ==(const FVertexBufferBinding& b) const
        {
            return Buffer == b.Buffer
                && Slot == b.Slot
                && Offset == b.Offset;
        }
        bool operator !=(const FVertexBufferBinding& b) const { return !(*this == b); }

        FVertexBufferBinding& SetBuffer(FRHIBuffer* value) { Buffer = value; return *this; }
        FVertexBufferBinding& SetSlot(uint32 value) { Slot = value; return *this; }
        FVertexBufferBinding& SetOffset(uint64 value) { Offset = value; return *this; }
    };

    struct FIndexBufferBinding
    {
        FRHIBuffer* buffer = nullptr;
        EFormat format;
        uint32 Offset;

        bool operator ==(const FIndexBufferBinding& b) const
        {
            return buffer == b.buffer
                && format == b.format
                && Offset == b.Offset;
        }
        bool operator !=(const FIndexBufferBinding& b) const { return !(*this == b); }

        FIndexBufferBinding& setBuffer(FRHIBuffer* value) { buffer = value; return *this; }
        FIndexBufferBinding& setFormat(EFormat value) { format = value; return *this; }
        FIndexBufferBinding& setOffset(uint32 value) { Offset = value; return *this; }
    };

    struct LUMINA_API FGraphicsState
    {
        FRHIGraphicsPipeline* Pipeline = nullptr;
        FRenderPassBeginInfo RenderPass = {};
        TVector<FRHIBindingSet*> Bindings;

        TVector<FVertexBufferBinding> VertexBuffers;
        FIndexBufferBinding IndexBuffer;

        FRHIBuffer* IndirectParams = nullptr;

        FGraphicsState& SetPipeline(FRHIGraphicsPipeline* value) { Pipeline = value; return *this; }
        //FGraphicsState& SetViewport(const ViewportState& value) { viewport = value; return *this; }
        FGraphicsState& AddBindingSet(FRHIBindingSet* value) { Bindings.push_back(value); return *this; }
        FGraphicsState& AddVertexBuffer(const FVertexBufferBinding& value) { VertexBuffers.push_back(value); return *this; }
        FGraphicsState& SetIndexBuffer(const FIndexBufferBinding& value) { IndexBuffer = value; return *this; }
        FGraphicsState& SetIndirectParams(FRHIBuffer* value) { IndirectParams = value; return *this; }
    };
    
    template <>
    struct FVertexTypeTraits<FVertex>
    {
        static constexpr uint32 Stride = sizeof(FVertex);
        static constexpr uint32 Binding = 0;

        static void GetVertexAttributes(TVector<FGraphicsPipelineSpec::FVertexAttribute>& OutAttributes)
        {
            FGraphicsPipelineSpec::FVertexAttribute Attribute;
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

        static void GetVertexAttributes(TVector<FGraphicsPipelineSpec::FVertexAttribute>& OutAttributes)
        {
            FGraphicsPipelineSpec::FVertexAttribute Attribute;
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
