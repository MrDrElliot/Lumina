#pragma once

#include <glm/glm.hpp>
#include "Core/Serialization/Archiver.h"

#define NO_TEXTURE (-1)

namespace Lumina
{

    struct FMiscData
    {
        const uint8* Data;
        int32 Size;
    };

    struct FMaterialAttributes
    {
        glm::vec4   baseColor = glm::vec4(1.0f);
        float       roughness = 0.5f;
        float       metallic = 0.5f;
        float       emissiveIntensity = 1.0f;
    };

    struct FMaterialTexturesData
    {
        int32 AlbedoID =        NO_TEXTURE;
        int32 NormalID =        NO_TEXTURE;
        int32 RoughnessID =     NO_TEXTURE;
        int32 EmissiveID =      NO_TEXTURE;
        int32 AOID =            NO_TEXTURE;
        int32 Padding[3] =      {0, 0, 0};
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
