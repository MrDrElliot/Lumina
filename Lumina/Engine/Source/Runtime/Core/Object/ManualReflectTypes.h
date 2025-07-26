#pragma once

#include "ObjectMacros.h"


#ifndef REFLECTION_PARSER

#include "Core/Math/Math.h"
#include "Core/Math/Transform.h"
#include "glm/glm.hpp"

#endif


#ifdef REFLECTION_PARSER

namespace glm
{
    
    LUM_STRUCT()
    struct vec2
    {
        LUM_PROPERTY(Editable)
        float x;

        LUM_PROPERTY(Editable)
        float y;
    };

    LUM_STRUCT()
    struct vec3
    {
        LUM_PROPERTY(Editable)
        float x;

        LUM_PROPERTY(Editable)
        float y;
    
        LUM_PROPERTY(Editable)
        float z;
    };

    LUM_STRUCT()
    struct vec4
    {
        LUM_PROPERTY(Editable)
        float x;

        LUM_PROPERTY(Editable)
        float y;
    
        LUM_PROPERTY(Editable)
        float z;

        LUM_PROPERTY(Editable)
        float w;
    };
}

namespace Lumina
{
    LUM_STRUCT()
    struct FTransform
    {
        LUM_PROPERTY(Editable)
        glm::vec3 Location;

        LUM_PROPERTY(Editable)
        glm::vec3 Rotation;

        LUM_PROPERTY(Editable)
        glm::vec3 Scale;
    };
}

#endif