#pragma once

#include <glm/glm.hpp>

struct FRenderableMeshlet
{
    glm::uint32 VertexOffset;
    glm::uint32 TriangleOffset;
    glm::uint32 VertexCount;
    glm::uint32 TriangleCount;
};