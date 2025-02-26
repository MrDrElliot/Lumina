#pragma once

#include <string>

#include "Core/Math/Math.h"
#include "Events/Event.h"

namespace Lumina
{
    struct FWindowSpecs
    {
        FString Title = "Lumina";
        FIntVector2D Extent;
        bool bFullscreen = false;

        struct FContext
        {
            TFunction<void(const FIntVector2D&)> ResizeCallback;
        } Context;
       
    };
}
