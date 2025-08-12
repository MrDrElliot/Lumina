#pragma once

#include "glm/glm.hpp"

namespace Lumina
{
    struct FAABB
    {
        glm::vec3 Min, Max;

        
        FAABB()
            : Min(0.0f), Max(0.0f)
        {}

        FAABB(const glm::vec3& InMin, const glm::vec3& InMax)
            : Min(InMin), Max(InMax)
        {}

        glm::vec3 GetSize() const { return Max - Min; }
        glm::vec3 GetCenter() const { return Min + GetSize() * 0.5f; }
        
        
    };
}