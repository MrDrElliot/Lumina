#pragma once

#include <cstddef>
#include <glm/glm.hpp>


namespace Lumina
{

    struct FMiscData
    {
        glm::uint8* Data;
        glm::int32 Size;
    };

    struct FVertex
    {
        glm::vec3 Position;
        float UV_X;
        glm::vec3 Normal;
        float UV_Y;
        glm::vec4 Color;
    };
    
}
