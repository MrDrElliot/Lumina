#pragma once
#include "RHIFwd.h"
#include "Types/BitFlags.h"

namespace Lumina
{
    enum class EDeviceBufferMemoryUsage;
    enum class EDeviceBufferUsage : uint32;

    class IRenderDevice
    {
    public:
        virtual ~IRenderDevice() = default;

        virtual void Initialize() = 0;

        virtual FRHICommandBufferHandle CreateCommandBuffer() = 0;

        virtual FRHIBufferHandle CreateBuffer(TBitFlags<EDeviceBufferUsage> UsageFlags, TBitFlags<EDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) = 0;
        virtual void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset = 0) = 0;
         

    private:
        
    };
}
