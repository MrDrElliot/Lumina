#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"
#include <Containers/String.h>

namespace Lumina
{

    enum class EDeviceBufferFlags : uint8
    {
        VERTEX_RATE =           1 << 0,
        INSTANCE_RATE =         1 << 1,
        INDEX_TYPE_UINT8 =      1 << 2,
        INDEX_TYPE_UINT16 =     1 << 3,
        INDEX_TYPE_UINT32 =     1 << 4,
        CREATE_STAGING_BUFFER = 1 << 5
    };

    enum class EDeviceBufferUsage : uint32
    {
        VERTEX_BUFFER,
        INDEX_BUFFER,
        UNIFORM_BUFFER,
        UNIFORM_BUFFER_DYNAMIC,
        STORAGE_BUFFER,
        STORAGE_BUFFER_DYNAMIC,
        STAGING_BUFFER,
        SHADER_DEVICE_ADDRESS,
        INDIRECT_PARAMS,
    };

    enum class EDeviceBufferMemoryUsage
    {
        READ_BACK,
        COHERENT_WRITE,
        NO_HOST_ACCESS
    };

    enum class EDeviceBufferMemoryHeap
    {
        DEVICE, // buffer is allocated in VRAM
        HOST	// buffer is allocated in system RAM
    };

    struct FDeviceBufferSpecification
    {
        uint64 Size;
        uint8 Flags;
        EDeviceBufferUsage BufferUsage;
        EDeviceBufferMemoryUsage MemoryUsage;
        EDeviceBufferMemoryHeap Heap;
        const char* DebugName;
    };
    
    class FBuffer : public RefCounted
    {
    public:
        
        virtual ~FBuffer() {}
        

        static TRefPtr<FBuffer> Create(const FDeviceBufferSpecification& Spec);
        static TRefPtr<FBuffer> Create(const FDeviceBufferSpecification& Spec, void* Data, uint64 DataSize);

        virtual void Destroy() = 0;


        virtual FDeviceBufferSpecification GetSpecification() const = 0;

        virtual void UploadData(uint64 Offset, void* Data, uint64 DataSize) = 0;
        
        virtual void Resize() = 0;
    };
}
