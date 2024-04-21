#pragma once
#include <string>

#include <glm/glm.hpp>

#include "Shader.h"

namespace Lumina
{
    
    struct DeviceBufferLayoutElement
    {
        std::string Name;
        EShaderDataType Format = EShaderDataType::FLOAT4;
        glm::uint32 Size = 0;
        glm::uint32 Offset = 0;

        DeviceBufferLayoutElement(const std::string& InName, EShaderDataType InFormat)
            : Name(InName), Format(InFormat) {}

        DeviceBufferLayoutElement(DeviceBufferLayoutElement& other)
        {
            Name = other.Name;
            Format = other.Format;
            Size = other.Size;
            Offset = other.Offset;
        }

        DeviceBufferLayoutElement(const DeviceBufferLayoutElement& other)
        {
            Name = other.Name;
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
                glm::uint32 datasize = DeviceDataTypeSize(element.Format);
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

        glm::uint32 GetStride() const { return Stride; }
        const std::vector<DeviceBufferLayoutElement>& GetElements() const { return Elements; }
        const std::vector<DeviceBufferLayoutElement>::iterator& begin() { return Elements.begin(); }
        const std::vector<DeviceBufferLayoutElement>::iterator& end() { return Elements.end(); }

    private:

        std::vector<DeviceBufferLayoutElement> Elements;
        glm::uint32 Stride = 0;
    };
}
