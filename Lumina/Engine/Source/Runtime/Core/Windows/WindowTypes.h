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

       std::function<void(FEvent&)> EventCallback;
    };
}
