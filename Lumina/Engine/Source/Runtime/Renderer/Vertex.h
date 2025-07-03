#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Math.h"
#include "Core/Templates/CanBulkSerialize.h"



namespace Lumina
{
    struct FVertex
    {
        glm::vec4       Position;
        glm::vec4       Color;
        glm::vec4       Normal;
        glm::vec2       UV;
        
        friend FArchive& operator<<(FArchive& Ar, FVertex& Data)
        {
            // Position
            Ar << Data.Position.x;
            Ar << Data.Position.y;
            Ar << Data.Position.z;
            Ar << Data.Position.w;

            // Color
            Ar << Data.Color.x;
            Ar << Data.Color.y;
            Ar << Data.Color.z;
            Ar << Data.Color.w;

            // Normal
            Ar << Data.Normal.x;
            Ar << Data.Normal.y;
            Ar << Data.Normal.z;
            Ar << Data.Normal.w;

            // UV
            Ar << Data.UV.x;
            Ar << Data.UV.y;

            return Ar;
        }
    };

    struct FSimpleElementVertex
    {
        glm::vec4   Position;
        glm::vec4      Color;
    };
    
    
    template<>
    struct TCanBulkSerialize<FVertex> { enum { Value = true }; };

    template<>
    struct TCanBulkSerialize<FSimpleElementVertex> { enum { Value = true }; };
    
}
