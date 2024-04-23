#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Lumina
{

    enum class EDeviceBufferFlags : glm::uint8
    {
        VERTEX_RATE =           1 << 0,
        INSTANCE_RATE =         1 << 1,
        INDEX_TYPE_UINT8 =      1 << 2,
        INDEX_TYPE_UINT16 =     1 << 3,
        INDEX_TYPE_UINT32 =     1 << 4,
        CREATE_STAGING_BUFFER = 1 << 5
    };

    enum class EDeviceBufferUsage : glm::uint32
    {
        VERTEX_BUFFER,
        INDEX_BUFFER,
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
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
        glm::uint64 Size;
        glm::uint8 Flags;
        EDeviceBufferUsage BufferUsage;
        EDeviceBufferMemoryUsage MemoryUsage;
        EDeviceBufferMemoryHeap Heap;
    };
    
    class FBuffer
    {
    public:
        
        virtual ~FBuffer() {}
        

        static std::shared_ptr<FBuffer> Create(const FDeviceBufferSpecification& Spec);
        static std::shared_ptr<FBuffer> Create(const FDeviceBufferSpecification& Spec, void* Data, glm::uint64 DataSize);


        virtual void Destroy() = 0;

        virtual FDeviceBufferSpecification GetSpecification() const = 0;

        virtual void UploadData(glm::uint64 Offset, void* Data, glm::uint64 DataSize) = 0;
    
    };
}