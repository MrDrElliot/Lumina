#pragma once

#include "Core/Math/Math.h"
#include "Events/Event.h"
#include "Core/Functional/Function.h"

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
