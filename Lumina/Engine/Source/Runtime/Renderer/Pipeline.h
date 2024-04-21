#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "BufferLayout.h"
#include "Image.h"
#include "Source/Runtime/GUID/GUID.h"

namespace Lumina
{
    class FShader;

    enum class EPipelineType : glm::uint8
    {
        GRAPHICS,
        COMPUTE,
        RAY_TRACING
    };

    enum class EPipelineCullingMode : glm::uint8
    {
        BACK,
        FRONT,
        NONE
    };

    enum class EPipelineFrontFace : glm::uint8
    {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    enum class EPipelineTopology : glm::uint8
    {
        TRIANGLES,
        LINES,
        POINTS
    };

    enum class EPipelineFillMode : glm::uint8
    {
        FILL,
        EDGE_ONLY
    };

    struct FPipelineSpecification
    {
        std::string debug_name;
        FDeviceBufferLayout input_layout;
        std::shared_ptr<FShader> shader;
        glm::float32 line_width;
        EPipelineType type;
        EPipelineCullingMode culling_mode;
        EPipelineFrontFace front_face;
        EPipelineTopology topology;
        EPipelineFillMode fill_mode;
        std::vector<EImageFormat> output_attachments_formats;
        bool primitive_restart_enable;
        bool color_blending_enable;
        bool depth_test_enable;
        bool multisampling_enable;
        glm::uint8 sample_count;

        static FPipelineSpecification Default()
        {
            FPipelineSpecification spec = {};
            spec.debug_name = "";
            spec.input_layout = {};
            spec.shader = nullptr;
            spec.line_width = 1.0f;
            spec.type = EPipelineType::GRAPHICS;
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

        bool operator== (const FPipelineSpecification& other) const
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

    
    class FPipeline
    {
    public:
        
        virtual ~FPipeline() = default;

        static std::shared_ptr<FPipeline> Create(const FPipelineSpecification& InSpec);
        virtual void Destroy() = 0;

        virtual const FPipelineSpecification& GetSpecification() const = 0;
        FGuid GetID() const { return Guid; }

    private:


    protected:
        
        FGuid Guid;
    
    };
}
