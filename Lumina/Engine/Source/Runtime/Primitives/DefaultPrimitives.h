#pragma once
#include <utility>
#include <vector>
#include <glm/glm.hpp>

#include "Source/Runtime/Renderer/RenderTypes.h"

namespace Lumina::Primitives
{
    inline std::pair<std::vector<FVertex>, std::vector<glm::uint32>> GenerateCube()
    {
        std::vector<FVertex> vertices =
        {
            {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // Red
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // Green
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // Blue
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}}, // Yellow
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}}, // Magenta
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}}, // Cyan
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // White
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 0.0f, 1.0f}}  // Black
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
