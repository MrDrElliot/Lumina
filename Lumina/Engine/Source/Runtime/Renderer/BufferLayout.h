#pragma once
#include <string>

#include <glm/glm.hpp>

#include "Shader.h"

namespace Lumina
{
    
    struct DeviceBufferLayoutElement
    {
        EShaderDataType Format = EShaderDataType::FLOAT4;
        uint32 Size = 0;
        uint32 Offset = 0;

        DeviceBufferLayoutElement(EShaderDataType InFormat)
            : Format(InFormat) {}

        DeviceBufferLayoutElement(DeviceBufferLayoutElement& other)
        {
            Format = other.Format;
            Size = other.Size;
            Offset = other.Offset;
        }

        DeviceBufferLayoutElement(const DeviceBufferLayoutElement& other)
        {
            Format = other.Format;
            Size = other.Size;
            Offset = other.Offset;
        }

        bool operator==(const DeviceBufferLayoutElement& other) const
        {
            bool result = true;
            result &= Format == other.Format;
            result &= Size == other.Size;
            result &= Offset == other.Offset;

            return result;
        }
    };

    class FDeviceBufferLayout
    {
    public:
        FDeviceBufferLayout() {}
        FDeviceBufferLayout(const std::vector<DeviceBufferLayoutElement>& list) : Elements(list)
        {
            for (auto& element : Elements)
            {
                uint32 datasize = DeviceDataTypeSize(element.Format);
                element.Offset = Stride;
                element.Size = datasize;
                Stride += datasize;
            }
        }
        
        FDeviceBufferLayout(FDeviceBufferLayout& other)
        {
            Stride = other.Stride;
            Elements = other.Elements;
        }

        FDeviceBufferLayout(const FDeviceBufferLayout& other)
        {
            Stride = other.Stride;
            Elements = other.Elements;
        }

        uint32 GetStride() const { return Stride; }
        const std::vector<DeviceBufferLayoutElement>& GetElements() const { return Elements; }
        const std::vector<DeviceBufferLayoutElement>::iterator begin() { return Elements.begin(); }
        const std::vector<DeviceBufferLayoutElement>::iterator end() { return Elements.end(); }

    private:

        std::vector<DeviceBufferLayoutElement> Elements;
        uint32 Stride = 0;
    };
}
