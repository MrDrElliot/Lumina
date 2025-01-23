#pragma once

#include <glm/glm.hpp>
#include "Renderer/Image.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    class FImage;

    struct FCameraData
    { 
        glm::mat4 View;
        glm::mat4 Projection;
    };

    struct FGBuffer
    {
        TRefPtr<FImage> BaseColor;
        TRefPtr<FImage> Normals;
        TRefPtr<FImage> RoughnessMetallic;
        TRefPtr<FImage> Emissive;
        TRefPtr<FImage> AmbientOcclusion;
    };
}
