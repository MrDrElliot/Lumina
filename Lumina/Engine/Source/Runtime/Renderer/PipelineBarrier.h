#pragma once
#include <memory>
#include <vector>

#include "glm/ext/scalar_uint_sized.hpp"

namespace Lumina
{
    class FImage;
    class FBuffer;
    enum class EImageLayout;


    struct FPipelineResourceBarrierInfo
    {
        glm::uint64 src_stages;
        glm::uint64 dst_stages;
        glm::uint64 src_access_mask;
        glm::uint64 dst_access_mask;
        EImageLayout new_image_layout;
        glm::uint64 buffer_barrier_size;
        glm::uint64 buffer_barrier_offset;
    };

    struct FPipelineBarrierInfo
    {
        std::vector<std::pair<std::shared_ptr<FBuffer>, FPipelineResourceBarrierInfo>> BufferBarriers;
        std::vector<std::pair<std::shared_ptr<FImage>, FPipelineResourceBarrierInfo>> ImageBarriers;
    };
}