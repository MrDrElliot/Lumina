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

       std::function<void(FEvent&)> EventCallback;
    };
}
