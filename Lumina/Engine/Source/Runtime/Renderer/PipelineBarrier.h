#pragma once

#include "Containers/Array.h"
#include "Buffer.h"
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FImage;
    class FBuffer;
    enum class EImageLayout;


    struct FPipelineResourceBarrierInfo
    {
        uint64 src_stages;
        uint64 dst_stages;
        uint64 src_access_mask;
        uint64 dst_access_mask;
        EImageLayout new_image_layout;
        uint64 buffer_barrier_size;
        uint64 buffer_barrier_offset;
    };

    struct FPipelineBarrierInfo
    {
        TFastVector<std::pair<TRefPtr<FBuffer>, FPipelineResourceBarrierInfo>> BufferBarriers;
        TFastVector<std::pair<TRefPtr<FImage>, FPipelineResourceBarrierInfo>> ImageBarriers;
    };
}
