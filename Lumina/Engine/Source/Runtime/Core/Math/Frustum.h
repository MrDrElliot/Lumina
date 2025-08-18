#pragma once

#include <glm/glm.hpp>

#include "AABB.h"
#include "Containers/Array.h"
#include "Core/Profiler/Profile.h"
#include "Platform/Platform.h"

namespace Lumina
{
    struct FFrustum
    {
        enum ESide { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5, NUM = 6};
        TArray<glm::vec4, NUM> Planes;

        NODISCARD bool IsInside(const glm::vec3& Point)
        {
            LUMINA_PROFILE_SCOPE();
            
            for (int i = 0; i < NUM; i++)
            {
                const glm::vec4& p = Planes[i];
                float dist = p.x * Point.x + p.y * Point.y + p.z * Point.z + p.w;
                if (dist < 0)
                {
                    return false;
                }
            }
            return true;
        }

        NODISCARD bool IsInside(const FAABB& aabb)
        {
            LUMINA_PROFILE_SCOPE();

            for (int i = 0; i < NUM; i++)
            {
                const glm::vec4& p = Planes[i];
                glm::vec3 normal(p.x, p.y, p.z);

                glm::vec3 positive;
                positive.x = (normal.x >= 0.0f) ? aabb.Max.x : aabb.Min.x;
                positive.y = (normal.y >= 0.0f) ? aabb.Max.y : aabb.Min.y;
                positive.z = (normal.z >= 0.0f) ? aabb.Max.z : aabb.Min.z;

                float dist = glm::dot(normal, positive) + p.w;

                if (dist < 0.0f)
                {
                    return false;
                }
            }

            return true;
        }
    };
}
