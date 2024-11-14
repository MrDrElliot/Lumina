#pragma once
#include "Math/Transform.h"

namespace Lumina
{
    struct TransformComponent
    {

        TransformComponent(const FTransform& InTransform) : Transform(InTransform) {};
    
        FTransform Transform;
    };
}