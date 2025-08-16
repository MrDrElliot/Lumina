#pragma once
#include "Containers/Array.h"
#include "Renderer/Vertex.h"


namespace Lumina::Primitives
{
    inline void GenerateCube(TVector<FVertex>& OutVertices, TVector<uint32>& OutIndices)
    {
        constexpr glm::vec4 white = glm::vec4(1.0f);
        const glm::vec4 normals[] = {
            { 0,  0,  1, 0}, { 0,  0, -1, 0}, {-1,  0,  0, 0},
            { 1,  0,  0, 0}, { 0,  1,  0, 0}, { 0, -1,  0, 0}
        };

        const glm::vec3 positions[24] = {
            // Front
            {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1},
            // Back
            {1, -1, -1}, {-1, -1, -1}, {-1, 1, -1}, {1, 1, -1},
            // Left
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, 1}, {-1, 1, -1},
            // Right
            {1, -1, 1}, {1, -1, -1}, {1, 1, -1}, {1, 1, 1},
            // Top
            {-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, -1},
            // Bottom
            {-1, -1, -1}, {1, -1, -1}, {1, -1, 1}, {-1, -1, 1},
        };

        const glm::vec2 uvs[4] = {
            {0, 0}, {1, 0}, {1, 1}, {0, 1}
        };

        OutVertices.clear();
        OutIndices.clear();
        OutVertices.reserve(24);

        for (int face = 0; face < 6; ++face)
        {
            for (int i = 0; i < 4; ++i)
            {
                int idx = face * 4 + i;
                OutVertices.push_back(
        {
                    glm::vec4(positions[idx], 1.0f),
                    white,
                    normals[face],
                    uvs[i]
                });
            }

            uint32_t base = face * 4;
            OutIndices.push_back(base + 0);
            OutIndices.push_back(base + 1);
            OutIndices.push_back(base + 2);
            OutIndices.push_back(base + 2);
            OutIndices.push_back(base + 3);
            OutIndices.push_back(base + 0);
        }
    }

}
