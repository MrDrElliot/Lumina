#pragma once

#include <string>
#include "Events/Event.h"

namespace Lumina
{
    struct FWindowSpecs
    {
        FString Title = "Lumina";
        uint32 Width =  0;
        uint32 Height = 0;

        bool bFullscreen = false;

       std::function<void(FEvent&)> EventCallback;
    };
}
