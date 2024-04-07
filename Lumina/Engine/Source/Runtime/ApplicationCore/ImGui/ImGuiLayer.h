#pragma once
#include "Source/Runtime/ApplicationCore/Layer.h"

namespace Lumina
{
    class FImGuiLayer : public FLayer
    {
    public:

        virtual void Begin() = 0;
        virtual void End() = 0;



        void SetupDarkThemeColors();
        void SetDarkThemeV2Colors();

        static FImGuiLayer* Create();
    
    };
}
