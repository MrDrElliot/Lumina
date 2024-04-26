#pragma once

#include <glm/glm.hpp>
namespace Lumina
{
    class FImage;

    struct FCameraData
    {
        glm::mat4 View;
        glm::mat4 Projection;
        glm::mat4 ViewProjection;
        glm::vec3 Position;
        glm::vec3 ForwardVector;
    };

    struct FGBuffer
    {
        std::shared_ptr<FImage> Positions;
        std::shared_ptr<FImage> Normals;
        std::shared_ptr<FImage> BaseColor;
        std::shared_ptr<FImage> MetallicRoughnessOcclusion;
    };
}