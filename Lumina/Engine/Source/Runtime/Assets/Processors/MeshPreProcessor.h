#pragma once
#include <vector>

#include "Source/Runtime/Renderer/Meshlet.h"

namespace Lumina
{

    struct FGeneratedMeshlets
    {
        std::vector<FRenderableMeshlet> Meshlets;
        std::vector<glm::uint32> Indices;
        std::vector<std::byte> LocalIndices;
    };
    
    class FMeshPreProcessor
    {
    public:

        FGeneratedMeshlets* GenerateMeshlets(const std::vector<std::byte>* Vertices, const std::vector<glm::uint32>* Indices, glm::uint32 VertexStride);

    
    };
}
