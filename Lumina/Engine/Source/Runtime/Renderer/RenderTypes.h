#pragma once

#include <glm/glm.hpp>
#include "Core/Serialization/Archiver.h"

namespace Lumina
{

    // @DEPRECATED 
    struct FMiscData
    {
        const uint8* Data;
        int32 Size;
    };

    struct FVertex
    {
        friend FArchive& operator << (FArchive& Ar, FVertex& data)
        {
            Ar << data.Position.x << data.Position.y << data.Position.z;
            Ar << data.Color.r << data.Color.g << data.Color.b << data.Color.a;
            Ar << data.UV.x << data.UV.y;
            Ar << data.Normal.x << data.Normal.y << data.Normal.z;
            return Ar;
        }
        
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 UV;
        glm::vec3 Normal;
        
    };
    
}
