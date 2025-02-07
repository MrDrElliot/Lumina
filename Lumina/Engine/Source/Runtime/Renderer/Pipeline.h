#pragma once

#include "BufferLayout.h"
#include "Image.h"
#include "Renderer/RenderResource.h"
#include "GUID/GUID.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FShader;

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

    struct FPipelineSpecification
    {
        FString DebugName;
        FDeviceBufferLayout input_layout;
        TRefPtr<FShader> shader;
        float line_width;
        EPipelineType type;
        EPipelineCullingMode culling_mode;
        EPipelineBlending SrcAlphaBlendFactor;
        EPipelineBlending DstAlphaBlendFactor;
        EPipelineFrontFace front_face;
        EPipelineTopology topology;
        EPipelineFillMode fill_mode;
        std::vector<EImageFormat> output_attachments_formats;
        bool primitive_restart_enable;
        bool color_blending_enable;
        bool depth_test_enable;
        bool multisampling_enable;
        uint8 sample_count;

        static FPipelineSpecification Default()
        {
            FPipelineSpecification spec = {};
            spec.DebugName = "";
            spec.input_layout = {};
            spec.shader = nullptr;
            spec.line_width = 1.0f;
            spec.type = EPipelineType::GRAPHICS;
            spec.SrcAlphaBlendFactor = EPipelineBlending::BLEND_FACTOR_ONE;
            spec.DstAlphaBlendFactor = EPipelineBlending::BLEND_FACTOR_ZERO;
            spec.culling_mode = EPipelineCullingMode::BACK;
            spec.front_face = EPipelineFrontFace::COUNTER_CLOCKWISE;
            spec.topology = EPipelineTopology::TRIANGLES;
            spec.fill_mode = EPipelineFillMode::FILL;
            spec.output_attachments_formats = {};
            spec.primitive_restart_enable = false;
            spec.color_blending_enable = true;
            spec.depth_test_enable = true;
            spec.multisampling_enable = false;
            spec.sample_count = 1;

            return spec;
        }

        bool operator == (const FPipelineSpecification& other) const
        {
            bool result = true;
            result &= shader == other.shader;
            result &= line_width == other.line_width;
            result &= type == other.type;
            result &= culling_mode == other.culling_mode;
            result &= front_face == other.front_face;
            result &= topology == other.topology;
            result &= fill_mode == other.fill_mode;
            result &= color_blending_enable == other.color_blending_enable;
            result &= depth_test_enable == other.depth_test_enable;
            result &= sample_count == other.sample_count;
            result &= input_layout.GetElements() == other.input_layout.GetElements();

            return result;
        }
    };

    
    class FPipeline : public FRenderResource
    {
    public:
        
        FPipeline() = default;
        virtual ~FPipeline() = default;

        static TRefPtr<FPipeline> Create(const FPipelineSpecification& InSpec);

        FORCEINLINE const FPipelineSpecification& GetSpecification() const { return Specification; }
        FORCEINLINE const FGuid& GetID() const { return Guid; }
        
        
        virtual void CreateGraphics() = 0;
        virtual void CreateCompute() = 0;

    protected:

        FPipelineSpecification Specification = {};
        FGuid Guid;
    
    };
}
