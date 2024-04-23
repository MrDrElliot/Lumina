#pragma once
#include <utility>
#include <vector>
#include <glm/glm.hpp>

namespace Lumina::Primitives
{
    inline std::pair<std::vector<glm::vec3>, std::vector<glm::uint32>> GenerateCube()
    {
        std::vector<glm::vec3> vertices = {
            {-0.5f, -0.5f,  0.5f },
            { 0.5f, -0.5f,  0.5f },
            { 0.5f,  0.5f,  0.5f },
            {-0.5f,  0.5f,  0.5f },
            {-0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f,  0.5f, -0.5f },
            {-0.5f,  0.5f, -0.5f }
        };

        std::vector<glm::uint32> indices = {
            0, 1, 2,
            2, 3, 0,
            4, 0, 3,
            3, 7, 4,
            5, 4, 7,
            7, 6, 5,
            1, 5, 6,
            6, 2, 1,
            3, 2, 6,
            6, 7, 3,
            0, 4, 5,
            5, 1, 0
        };

        return { vertices, indices };
    }
}
