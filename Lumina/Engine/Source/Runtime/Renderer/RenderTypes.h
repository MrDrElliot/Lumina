#pragma once

#include <glm/glm.hpp>


namespace Lumina
{

    struct FMiscData
    {
        const glm::uint8_t* Data;
        glm::int32 Size;
    };

    struct FVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 UV;
    };
    
}
