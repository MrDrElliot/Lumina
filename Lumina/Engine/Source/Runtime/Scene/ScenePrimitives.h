#pragma once

#include <glm/glm.hpp>
namespace Lumina
{
    struct FCameraData
    {
        glm::mat4 View;
        glm::mat4 Projection;
        glm::mat4 ViewProjection;
        glm::vec3 Position;
        glm::vec3 ForwardVector;
    };
}