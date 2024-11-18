#pragma once

#include <string>
#include "Events/Event.h"

namespace Lumina
{
    struct FWindowSpecs
    {
        std::string Title = "Lumina";
        uint32 Width = 1600;
        uint32 Height = 900;

        bool bFullscreen = false;

       std::function<void(FEvent&)> EventCallback;
    };
}
